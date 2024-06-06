#include "../headers/ChatClient.h"

ChatClient::ChatClient(string address, int port, string name)
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
}

ChatClient::~ChatClient()
{
    closesocket(clientSocket);
}

void ChatClient::Send()
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
    }

    Disconnect();
}

void ChatClient::Receive()
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
    }

    Disconnect();
}

void ChatClient::Disconnect()
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