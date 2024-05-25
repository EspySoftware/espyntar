#include <iostream>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>

using std::cout;
using std::endl;
using std::map;
using std::string;

#pragma comment(lib, "ws2_32.lib")

bool InitWinsock()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

class ChatServer
{
public:
    int port;
    SOCKET listenSocket;
    sockaddr_in serveraddr;
    map<SOCKET, string> clients;

    ChatServer(int port = 69420)
    {
        this->port = port;

        listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket == INVALID_SOCKET)
        {
            cout << "Failed to create socket." << endl;
            return;
        }

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(port);

        if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
        {
            cout << "Invalid IP address." << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Bind the socket to the address
        if (bind(listenSocket, reinterpret_cast<sockaddr *>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
        {
            cout << "Failed to bind the socket." << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Listen for incoming connections
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            cout << "Failed to listen on socket." << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        cout << "Listening on port: " << port << endl;
    }

    ~ChatServer()
    {
        closesocket(listenSocket);

        for (auto const &client : clients)
        {
            closesocket(client.first);
        }

        WSACleanup();
    }

    void AddClient(SOCKET clientSocket, string name)
    {
        clients[clientSocket] = name;

        cout << "Client connected: " << name << endl;
    }

    void RemoveClient(SOCKET clientSocket)
    {
        string name = clients[clientSocket];
        clients.erase(clientSocket);

        cout << "Client disconnected: " << name << endl;
    }
};

int main()
{
    if (!InitWinsock())
    {
        cout << "Failed to initialize Winsock." << endl;
        return -1;
    }

    system("clear");
    cout << R"(
     ______                       _             
    |  ____|                     | |            
    | |__   ___ _ __  _   _ _ __ | |_ __ _ _ __ 
    |  __| / __|  _ \| | | |  _ \| __/ _  |  __|
    | |____\__ \ |_) | |_| | | | | || (_| | |   
    |______|___/  __/ \__, |_| |_|\__\__,_|_|   
               | |     __/ |                    
               |_|    |___/                     
    )" << endl;

    ChatServer server(69420);

    // Accept incoming connections
    SOCKET clientSocket = accept(server.listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Failed to accept incoming connection." << endl;
        closesocket(server.listenSocket);
        WSACleanup();
        return -1;
    }

    // Receive name from the client
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    string name(buffer, bytesReceived);
    server.AddClient(clientSocket, name);

    // Send data to the client
    string message = "Welcome to the chat server, " + name + "!";
    send(clientSocket, message.c_str(), message.size() + 1, 0);

    return 0;
}