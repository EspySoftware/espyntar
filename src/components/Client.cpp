#include "../headers/Client.h"
// #include "../headers/Painter.h"

Client::Client(string address, int port, string name)
{
    this->name = name;

    // Create a socket
    this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Failed to create socket." << endl;
        return;
    }

    // Connect to the server
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &(serverAddress.sin_addr));

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        cout << "Failed to connect to the server." << endl;
        cout << "Error code: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    cout << "Connected to the server at " << address << ":" << port << endl;

    // Send name to the server
    int bytesent = send(clientSocket, name.c_str(), name.length(), 0);
    if (bytesent == SOCKET_ERROR)
    {
        cout << "Failed to send data to the server." << endl;
        return;
    }

    /* Process the server response */

    // Receive the client ID from the server
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Failed to receive data from the server." << endl;
        return;
    }

    buffer[bytesReceived] = '\0';
    id = stoi(buffer);
    cout << "\nConnected as [" << id << "]: " << name << endl;
    messages.push_back("Connected as " + name);

    // Receive the list of connected clients from the server
    bytesReceived = recv(clientSocket, buffer, 4096, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Failed to receive data from the server." << endl;
        return;
    }

    // Ensure the buffer is null-terminated
    buffer[bytesReceived] = '\0';

    // Extract the ADMIN and PAINTER ids
    string adminPainter(buffer);
    cout << "Received: " << adminPainter << endl;

    regex adminRegex("ADMIN:\\s+(\\d+),\\s+PAINTER:\\s+(\\d+)");
    smatch adminMatch;

    if (regex_search(adminPainter, adminMatch, adminRegex) && adminMatch.size() > 2)
    {
        adminID = stoi(adminMatch[1].str());
        painterID = stoi(adminMatch[2].str());

        cout << "ADMIN ID: " << adminID << endl;
        cout << "PAINTER ID: " << painterID << endl;
    }

    // Add connected clients to the vector
    // Format: "Connected clients: [1]Client1(100), [2]Client2(0), [3]Client3(200)"
    string connectedClients = adminPainter.substr(adminPainter.find("Connected clients: ") + 19); // the + 19 is to skip the "Connected clients: " part

    // Split the string by commas
    stringstream ss(connectedClients);
    string client;
    vector<string> clients;
    while (getline(ss, client, ','))
    {
        // Trim leading and trailing spaces
        client.erase(0, client.find_first_not_of(" \t\n\r\f\v"));
        client.erase(client.find_last_not_of(" \t\n\r\f\v") + 1);

        // Regular expression to match the client format
        regex clientRegex("\\[(\\d+)\\](\\w+)\\((\\d+)\\)");
        smatch matches;
        if (regex_search(client, matches, clientRegex))
        {
            int id = stoi(matches[1].str());
            string name = matches[2].str();
            int points = stoi(matches[3].str());

            // Add the client to the vector
            this->connectedClients.push_back({id, name, points});
            cout << "Client: [" << id << "] " << name << " (" << points << ")" << endl;
        }
        else
        {
            cout << "Failed to parse client: " << client << endl;
        }
    }
}

Client::~Client()
{
    closesocket(clientSocket);
}

void Client::Send()
{
    string message;

    while (true)
    {
        getline(cin, message);
        string msg = "[" + name + "]: " + message;

        int bytesent = send(clientSocket, message.c_str(), message.length(), 0);

        if (bytesent == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            cout << "Failed to send data to the server. Error code: " << errorCode << endl;
            break;
        }

        if (message == "exit")
        {
            break;
        }

        messages.push_back(msg);

        // // Debug print
        // cout << "Current messages after sending: ";
        // for (const auto &m : messages)
        // {
        //     cout << m << " ";
        // }
        // cout << endl;
    }

    Disconnect();
}

void Client::Send(string message)
{
    // Not a PAINT or POINTS or ANSWER command or PAINTER command or ADMIN command or START_GAME command or ROUND_OVER command
    if (message.find("PAINT:") != 0 && message.find("POINTS:") != 0 && message.find("ANSWER:") != 0 && message.find("PAINTER:") != 0 && message.find("ADMIN:") != 0 && message.find("START_GAME") != 0 && message.find("ROUND_OVER") != 0)
    {
        // Regular message
        // Format: "(ID) [name]: message"
        string msg = "(" + to_string(id) + ") [" + name + "]: " + message;
        messages.push_back(msg);
    }

    // Send the message to the server
    int bytesent = send(clientSocket, message.c_str(), message.length(), 0);
    if (bytesent == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        cout << "Failed to send data to the server. Error code: " << errorCode << endl;
    }

    // // Debug print
    // cout << "Current messages after sending: ";
    // for (const auto &m : messages)
    // {
    //     cout << m << " ";
    // }
    // cout << endl;
}

void Client::Receive()
{
    char buffer[4096];
    int bytesReceived;

    while (true)
    {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
        {
            int errorCode = WSAGetLastError();
            cout << "Disconnected from the server. Error code: " << errorCode << endl;
            break;
        }

        /* Process the server response */
        buffer[bytesReceived] = '\0';
        string message = buffer;

        // PAINT COMMAND
        // Format: "PAINT:100,200,1,10" (x, y, color, brushSize)
        if (message[0] == 'P' && message.find("PAINT:") == 0)
        {
            // Extract the paint data
            regex r("PAINT:(\\d+),(\\d+),(\\d+),(\\d+)");
            smatch match;
            if (regex_search(message, match, r) && match.size() > 4)
            {
                int x = stoi(match.str(1));
                int y = stoi(match.str(2));
                int color = stoi(match.str(3));
                int brushSize = stoi(match.str(4));

                // Add the paint message to the vector
                paintMessages.push_back(PaintMessage{x, y, color, static_cast<float>(brushSize)});
            }

            continue;
        }

        // ANSWER command
        // Format: "ANSWER: WORD"
        if (message[0] == 'A' && message.find("ANSWER:") == 0)
        {
            string word = message.substr(8);
            cout << "The word is: " << word << endl;
            chosenWord = word;

            messages.push_back("Word has been chosen.");

            continue;
        }

        // POINTS command
        // Format: "(id) points: 100."
        regex pointsRegex("(\\d+)\\)\\s+points:\\s+(\\d+).");
        smatch pointsMatch;
        if (regex_search(message, pointsMatch, pointsRegex) && pointsMatch.size() > 2)
        {
            int id = stoi(pointsMatch.str(1));
            int points = stoi(pointsMatch.str(2));
            cout << "Server: Giving client ( " << id << " ) " << name << " " << points << " points." << endl;

            // Update the points of the client
            for (int i = 0; i < connectedClients.size(); i++)
            {
                if (connectedClients[i].id == id)
                {
                    connectedClients[i].points += points;
                    connectedClients[i].guessedCorrectly = true;
                    break;
                }
            }

            continue;
        }

        // Add and remove clients from the vector
        // Format: (1) Client1 disconnected. or (22) Client22 connected. (add id and name to the vector)
        regex clientsRegex("(\\d+)\\)\\s+(\\w+)");
        smatch clientsMatch;
        if (regex_search(message, clientsMatch, clientsRegex) && clientsMatch.size() > 2)
        {
            int id = stoi(clientsMatch.str(1));
            string name = clientsMatch.str(2);

            // Client disconnected
            if (message.find("disconnected") != string::npos)
            {
                for (int i = 0; i < connectedClients.size(); i++)
                {
                    if (connectedClients[i].id == id)
                    {
                        connectedClients.erase(connectedClients.begin() + i);
                        break;
                    }
                }
            }
            // Client connected
            else if (message.find("connected") != string::npos)
            {
                connectedClients.push_back({id, name, 0});
            }
        }

        // PAINTER command
        // Format: "PAINTER: 1"
        regex painterRegex("PAINTER:\\s+(\\d+)");
        smatch painterMatch;
        if (regex_search(message, painterMatch, painterRegex) && painterMatch.size() > 1)
        {
            // Extract the painter ID
            painterID = stoi(painterMatch.str(1));
            cout << "The painter is: " << painterID << endl;

            // Update the painter ID in the connected clients vector
            for (int i = 0; i < connectedClients.size(); i++)
            {
                if (connectedClients[i].id == painterID)
                {
                    cout << "The painter is: " << connectedClients[i].name << endl;
                    break;
                }
            }

            continue;
        }

        // START_GAME command
        // Format: "START_GAME"
        if (message.find("START_GAME") == 0)
        {
            cout << "The game has started." << endl;
            messages.push_back("The game has started.");

            continue;
        }

        // ROUND_OVER command
        // Format: "ROUND_OVER"
        if (message.find("ROUND_OVER") == 0)
        {
            cout << "The round is over." << endl;

            this->round_over = true;
            this->chosenWord = "";

            // Reset the guessedCorrectly flag for all clients
            for (int i = 0; i < connectedClients.size(); i++)
            {
                this->connectedClients[i].guessedCorrectly = false;
            }
            this->guessed = false;
            
            continue;
        }

        // Regular message
        // Format: "(ID) [name]: message"
        regex msgRegex("\\((\\d+)\\)\\s+\\[(\\w+)\\]:\\s+(.*)");
        smatch msgMatch;

        if (regex_search(message, msgMatch, msgRegex) && msgMatch.size() > 3)
        {
            int id = stoi(msgMatch.str(1));
            string name = msgMatch.str(2);
            string msg = msgMatch.str(3);

            cout << "REGULAR MESSAGE" << endl;
        }

        cout << buffer << endl;
        messages.push_back(buffer);
    }

    Disconnect();
}

void Client::AddPoints(int points)
{
    cout << "Correct answer, receiving " << points << " points." << endl;

    // Update the points of the client
    for (int i = 0; i < connectedClients.size(); i++)
    {
        if (connectedClients[i].id == id)
        {
            connectedClients[i].points += points;
            break;
        }
    }

    // Send the points to the server
    stringstream ss;
    ss << "POINTS: " << points;
    Send(ss.str());
}

void Client::Disconnect()
{
    closesocket(clientSocket);
}

bool InitWinsock()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        cout << "WSAStartup failed: " << result << endl;
        return false;
    }
    return true;
}