#include "../headers/ChatClient.h"

ChatClient::ChatClient(string address, int port, string name)
{
    this->name = name;

    // Create a socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Failed to create socket." << endl;
        return;
    }

    // Connect to the server
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, address.c_str(), &(serveraddr.sin_addr));

    if (connect(clientSocket, reinterpret_cast<sockaddr *>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
    {
        cout << "Failed to connect to the server." << endl;
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
    WSACleanup();
}

void ChatClient::Send(string message)
{
    int bytesent = send(clientSocket, message.c_str(), message.length(), 0);

    if (bytesent == SOCKET_ERROR)
    {
        cout << "Failed to send data to the server." << endl;
    }

    cout << "Sent: " << message << endl;
}

string ChatClient::Receive()
{
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, 4096, 0);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "Failed to receive data from the server." << endl;
        return "";
    }

    buffer[bytesReceived] = '\0';
    return string(buffer);
}

bool InitWinsock()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}