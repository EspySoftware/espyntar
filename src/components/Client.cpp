#include "../headers/Client.h"

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
    // Example string: Connected clients: Client1, Client2, Client3
    stringstream ss(connectedClients.substr(19)); // Skip the "Connected clients: " part
    string clientName;
    while (getline(ss, clientName, ','))
    {
        // Remove leading and trailing whitespace
        clientName.erase(clientName.begin(), std::find_if(clientName.begin(), clientName.end(), [](unsigned char ch)
                                                          { return !std::isspace(ch); }));

        clientName.erase(std::find_if(clientName.rbegin(), clientName.rend(), [](unsigned char ch)
                                      { return !std::isspace(ch); })
                             .base(),
                         clientName.end());

        this->connectedClients.push_back(clientName);
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
    string msg = "[" + name + "]: " + message;

    int bytesent = send(clientSocket, message.c_str(), message.length(), 0);

    if (bytesent == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        cout << "Failed to send data to the server. Error code: " << errorCode << endl;
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

        buffer[bytesReceived] = '\0';
        cout << buffer << endl;

        messages.push_back(buffer);

        // Add newly connected client to the vector
        // Example string: [Client1] has connected.
        if (string(buffer).find("has connected.") != string::npos)
        {
            string clientName = string(buffer).substr(1, string(buffer).find("]") - 1);
            connectedClients.push_back(clientName);
        }

        // Remove disconnected client from the vector
        // Example string: [Client1] has disconnected.
        if (string(buffer).find("has disconnected.") != string::npos)
        {
            string clientName = string(buffer).substr(1, string(buffer).find("]") - 1);
            connectedClients.erase(std::remove(connectedClients.begin(), connectedClients.end(), clientName), connectedClients.end());
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