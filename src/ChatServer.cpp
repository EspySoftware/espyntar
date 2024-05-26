#include <iostream>
#include <string>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::to_string;

#pragma comment(lib, "ws2_32.lib")

bool InitWinsock()
{
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

typedef struct _client
{
    int id;
    string name;
} Client;

class ChatServer
{
public:
    int clientCount = 0;
    int port;
    SOCKET listenSocket;
    sockaddr_in serveraddr;
    map<SOCKET, Client> clients;

    ChatServer(int port = 12345)
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

        cout << "Listening on address: " << inet_ntoa(serveraddr.sin_addr) << ":" << port << endl;
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
        Client client;
        client.id = clientCount++;
        client.name = name;

        clients[clientSocket] = client;

        // Send the client ID to the client
        string id = to_string(client.id);
        send(clientSocket, id.c_str(), id.length(), 0);

        cout << "Client connected: " << "[" << client.id << "] " << client.name << endl;
    }

    void RemoveClient(SOCKET clientSocket)
    {
        Client client = clients[clientSocket];
        cout << "Client disconnected: " << client.name << endl;

        closesocket(clientSocket);
        clients.erase(clientSocket);
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

    ChatServer server;

    // Accept incoming connections
    SOCKET clientSocket = accept(server.listenSocket, nullptr, nullptr);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Failed to accept incoming connection." << endl;
        closesocket(server.listenSocket);
        WSACleanup();
        return -1;
    }

    // Receive name from the client and send the client ID
    char buffer[4096];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    string name(buffer, bytesReceived);
    server.AddClient(clientSocket, name);

    return 0;
}