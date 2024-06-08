#include "../headers/Client.h"
#include "../headers/Painter.h"

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

    buffer[bytesReceived] = '\0';
    string connectedClients(buffer);

    // Add connected clients to the vector
    // Format: "Connected clients: [1]Client1(100), [2]Client2(0), [3]Client3(200)"

    // Remove the "Connected clients: " prefix
    string clientsString = connectedClients.substr(19);

    // Split the string by commas
    stringstream ss(clientsString);
    string client;
    vector<string> clients;
    while (getline(ss, client, ','))
    {
        clients.push_back(client);
    }

    // Regular expression to match the client format
    regex clientRegex("\\[(\\d+)\\](\\w+)\\((\\d+)\\)");

    cout << "Connected clients:" << endl;
    for (const string &client : clients)
    {
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
    // PAINT command
    if (message.find("PAINT:") != 0)
    {
        string msg = "[" + name + "]: " + message;
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
        if (message.find("PAINT:") == 0)
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

                // Draw the paint action
                painter->Paint({(float)x, (float)y}, color, brushSize);
            }
        }
        else
        {
            // Add and remove clients from the vector
            // Format: (1) Client1 disconnected. or (22) Client22 connected. (add id and name to the vector)
            regex r("(\\d+)\\)\\s+(\\w+)");
            smatch match;

            if (regex_search(message, match, r) && match.size() > 2)
            {
                int id = stoi(match.str(1));
                string name = match.str(2);

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

            cout << buffer << endl;
            messages.push_back(buffer);
        }
    }

    Disconnect();
}

vector<string> Client::getMessages()
{
    return messages;
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