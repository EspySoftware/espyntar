#include <iostream>
#include <string>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include "./headers/ChatClient.h"

#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::thread;
using std::to_string;

class ChatServer
{
public:
    int clientCount = 0;
    int port;
    SOCKET listenSocket;
    sockaddr_in serverAddress;
    map<int, ChatClient> clients;

    ChatServer(int port = 12345)
    {
        this->port = port;

        listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket == INVALID_SOCKET)
        {
            cout << "Failed to create socket. Error: " << WSAGetLastError() << endl;
            return;
        }

        // // Get the local IP address
        // char hostName[256];
        // if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        // {
        //     cout << "Failed to get host name. Error: " << WSAGetLastError() << endl;
        //     closesocket(listenSocket);
        //     WSACleanup();
        //     return;
        // }

        // struct hostent *hostInfo = gethostbyname(hostName);
        // if (hostInfo == NULL)
        // {
        //     cout << "Failed to get host info. Error: " << WSAGetLastError() << endl;
        //     closesocket(listenSocket);
        //     WSACleanup();
        //     return;
        // }

        // // Use the first IP address returned
        // serverAddress.sin_addr.s_addr = *(u_long *)hostInfo->h_addr_list[0];
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);

        // Bind the socket to the specific local network IP address
        if (InetPton(AF_INET, _T("192.168.100.19"), &serverAddress.sin_addr) != 1) // Replace with desired local IP
        {
            cout << "Invalid IP address. Error: " << WSAGetLastError() << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Bind the socket to the address
        if (bind(listenSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
        {
            cout << "Failed to bind the socket. Error: " << WSAGetLastError() << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Set the socket to listen for incoming connections
        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
        {
            cout << "Failed to listen on socket. Error: " << WSAGetLastError() << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        cout << "Listening on address: " << inet_ntoa(serverAddress.sin_addr) << ":" << port << endl;
    }

    ~ChatServer()
    {
        if (closesocket(listenSocket) == SOCKET_ERROR)
        {
            int errorCode = WSAGetLastError();
            cout << "Failed to close listen socket. Error code: " << errorCode << endl;
            // Handle error based on errorCode
        }

        for (auto const &client : clients)
        {
            if (closesocket(client.second.clientSocket) == SOCKET_ERROR)
            {
                int errorCode = WSAGetLastError();
                cout << "Failed to close client socket. Error code: " << errorCode << endl;
            }
        }

        WSACleanup();
    }

    void Broadcast(ChatClient &sender, string message)
    {
        for (auto const &otherClient : clients)
        {
            if (otherClient.first != sender.id)
            {
                if (otherClient.second.clientSocket != INVALID_SOCKET) // Check if the client is still connected
                {
                    send(otherClient.second.clientSocket, message.c_str(), message.length(), 0);
                }
            }
        }
    }

    ChatClient AddClient(SOCKET clientSocket, string name)
    {
        clientCount++;

        ChatClient client(clientCount, name, clientSocket);

        clients[clientCount] = client;

        // Send the client ID to the client
        string id = to_string(client.id);
        send(clientSocket, id.c_str(), id.length(), 0);

        cout << "Client connected: " << "[" << client.id << "] " << client.name << endl;

        // broadcast connection message
        string msg = "[" + client.name + "] has connected.";
        Broadcast(client, msg);

        return client;
    }

    void RemoveClient(ChatClient client)
    {
        clients.erase(client.id);
        cout << "Client disconnected: " << "[" << client.id << "] " << client.name << endl;

        // broadcast disconnection message
        string msg = "[" + client.name + "] has disconnected.";
        Broadcast(client, msg);
    }

    void InteractWithClient(SOCKET csocket)
    {
        // Receive name from the client and send the client ID
        char buffer[4096];
        int bytesReceived = recv(csocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
        {
            cout << "Failed to receive data from client. Error: " << WSAGetLastError() << endl;
            return;
        }
        string name(buffer, bytesReceived);

        ChatClient client = AddClient(csocket, name);

        while (true)
        {
            bytesReceived = recv(client.clientSocket, buffer, sizeof(buffer), 0);

            if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
            {
                break;
            }

            string message(buffer, bytesReceived);
            if (message == "exit")
            {
                break;
            }
            cout << "[" << client.id << "] " << client.name << ": " << message << endl;

            string msg = "[" + client.name + "]: " + message;
            Broadcast(client, msg);
        }

        RemoveClient(client);
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

    while (true)
    {
        // Accept incoming connections
        SOCKET clientSocket = accept(server.listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            int errorCode = WSAGetLastError();
            cout << "Failed to accept incoming connection. Error code: " << errorCode << endl;
            break;
        }

        // Interact with the client
        thread t1([&server, clientSocket]()
                  { server.InteractWithClient(clientSocket); });

        t1.detach();
    }

    return 0;
}
