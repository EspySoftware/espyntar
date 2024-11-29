#include "../headers/Client.h"
#include "../headers/SocketFactory.h"
#include "../client/PaintMessageHandler.h"
#include "../client/AnswerMessageHandler.h"
#include "../client/PointsMessageHandler.h"
#include "../client/PainterMessageHandler.h"
#include "../client/AdminMessageHandler.h"
#include "../client/StartGameMessageHandler.h"
#include "../client/RoundOverMessageHandler.h"
#include "../client/ClientStatusMessageHandler.h"

Client::Client(string address, int port, string name)
{
    this->name = name;
    // Initialize handlers
    handlers["PAINT"] = std::make_unique<PaintMessageHandler>();
    handlers["ANSWER"] = std::make_unique<AnswerMessageHandler>();
    handlers["POINTS"] = std::make_unique<PointsMessageHandler>();
    handlers["PAINTER"] = std::make_unique<PainterMessageHandler>();
    handlers["ADMIN"] = std::make_unique<AdminMessageHandler>();
    handlers["START_GAME"] = std::make_unique<StartGameMessageHandler>();
    handlers["ROUND_OVER"] = std::make_unique<RoundOverMessageHandler>();
    handlers["CLIENT_STATUS"] = std::make_unique<ClientStatusMessageHandler>();


    // Crear y conectar el socket usando SocketFactory
    this->clientSocket = SocketFactory::CreateSocket();
    if (this->clientSocket == INVALID_SOCKET)
    {
        return;
    }

    if (!SocketFactory::ConnectSocket(this->clientSocket, address, port, serverAddress))
    {
        return;
    }

    // Enviar nombre al servidor
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

    // Add connected clients to the vector and set ADMIN and PAINTER IDs
    // Format: "Connected clients: [1]Client1(100) [ADMIN], [2]Client2(0), [3]Client3(200)"
    string clients(buffer);
    cout << "Received: " << clients << endl;
    string connectedClientsStr = clients.substr(clients.find("Connected clients: ") + 19); // the + 19 is to skip the "Connected clients: " part

    // Split the string by commas
    stringstream ss(connectedClientsStr);
    string client;
    while (getline(ss, client, ','))
    {
        // Extract the client ID, name, and points
        regex r("\\[(\\d+)\\](\\w+)\\((\\d+)\\)");
        smatch match;
        if (regex_search(client, match, r) && match.size() > 3)
        {
            int id = stoi(match.str(1));
            string name = match.str(2);
            int points = stoi(match.str(3));

            // Add the client to the vector
            connectedClients.push_back(OtherClient{id, name, points});

            // Check if the client is the ADMIN
            if (client.find("[ADMIN]") != string::npos)
            {
                adminID = id;
                painterID = id;
            }
        }
    }
}

Client::~Client()
{
    closesocket(clientSocket);
}

void Client::handleMessage(const std::string& message) {
    if (message.find("PAINT:") == 0) {
        handlers["PAINT"]->handle(*this, message);
    }
    else if (message.find("ANSWER:") == 0) {
        handlers["ANSWER"]->handle(*this, message);
    }
    else if (message.find("PAINTER:") == 0) {
        handlers["PAINTER"]->handle(*this, message);
    }
    else if (message.find("ADMIN:") == 0) {
        handlers["ADMIN"]->handle(*this, message);
    }
    else if (message.find("START_GAME") == 0) {
        handlers["START_GAME"]->handle(*this, message);
    }
    else if (message.find("ROUND_OVER") == 0) {
        handlers["ROUND_OVER"]->handle(*this, message);
    }
    else if (message.find("(") == 0) {
        if (message.find("points:") != std::string::npos) {
            handlers["POINTS"]->handle(*this, message);
        }
        else {
            // Regex for client status
            regex statusRegex(R"(\(\d+\)\s+\w+\s+(connected|disconnected)\.)");
            if (regex_match(message, statusRegex)) {
                handlers["CLIENT_STATUS"]->handle(*this, message);
            }
            else {
                messages.push_back(message);
            }
        }
    }
    else {
        messages.push_back(message);
    }
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
        handleMessage(message);
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