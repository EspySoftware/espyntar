#include <iostream>
#include <string>
#include <map>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include <sstream>
#include "./headers/Client.h"

#pragma comment(lib, "Ws2_32.lib")

using std::cout;
using std::endl;
using std::map;
using std::string;
using std::stringstream;
using std::thread;
using std::to_string;

class Server
{
public:
    int clientCount = 0;
    int port;
    SOCKET listenSocket;
    sockaddr_in serverAddress;
    map<int, Client> clients;

    bool gameStarted = false;
    int painterID = -1;
    int adminID = -1;
    string chosenWord;

    Server(int port = 12345)
    {
        this->port = port;

        listenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (listenSocket == INVALID_SOCKET)
        {
            cout << "Failed to create socket. Error: " << WSAGetLastError() << endl;
            return;
        }

        // Get the local IP address
        char hostName[256];
        if (gethostname(hostName, sizeof(hostName)) == SOCKET_ERROR)
        {
            cout << "Failed to get host name. Error: " << WSAGetLastError() << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        struct hostent *hostInfo = gethostbyname(hostName);
        if (hostInfo == NULL)
        {
            cout << "Failed to get host info. Error: " << WSAGetLastError() << endl;
            closesocket(listenSocket);
            WSACleanup();
            return;
        }

        // Use the first IP address returned
        serverAddress.sin_addr.s_addr = *(u_long *)hostInfo->h_addr_list[0];
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);

        // // Bind the socket to the specific local network IP address
        // if (InetPton(AF_INET, _T("192.168.100.19"), &serverAddress.sin_addr) != 1) // Replace with desired local IP
        // {
        //     cout << "Invalid IP address. Error: " << WSAGetLastError() << endl;
        //     closesocket(listenSocket);
        //     WSACleanup();
        //     return;
        // }

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

    ~Server()
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

    void Broadcast(Client &sender, string message)
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

    Client AddClient(SOCKET clientSocket, string name)
    {
        clientCount++;

        Client client(clientCount, name, clientSocket);

        clients[clientCount] = client;

        // Send the client ID to the client
        string id = to_string(client.id);
        send(clientSocket, id.c_str(), id.length(), 0);

        // Format: "(1) Client1 connected."
        string msg = "(" + to_string(client.id) + ") " + client.name + " connected.";

        // broadcast connection message with the client's id and name
        cout << msg << endl;
        Broadcast(client, msg);

        // Create a string with the id, name and points of all connected clients
        // Format: "Connected clients: [1]Client1(100), [2]Client2(0), [3]Client3(200)"
        string connectedClients = "Connected clients: ";
        int i = 0; // Counter to keep track of the number of connected clients
        for (const auto &otherClient : clients)
        {
            // Check if the client is still connected
            if (otherClient.second.clientSocket != INVALID_SOCKET)
            {
                connectedClients += "[" + to_string(otherClient.second.id) + "]" + otherClient.second.name + "(" + to_string(otherClient.second.points) + "), ";
                i++;
            }
        }

        if (i == 1)
        {
            client.adminID = client.id;
            client.painterID = client.id;

            adminID = client.id;
            painterID = client.id;
        }

        // Remove the trailing comma and space
        if (connectedClients.length() > 0)
        {
            connectedClients = connectedClients.substr(0, connectedClients.length() - 2);
        }

        // Send current admin and painter
        // Formar: "ADMIN: 1, PAINTER: 2"
        stringstream ss;
        ss << "ADMIN: " << adminID << ", PAINTER: " << painterID;
        string adminPainter = ss.str();
        send(clientSocket, adminPainter.c_str(), adminPainter.length(), 0);

        // Send the list of connected clients to the new client
        send(clientSocket, connectedClients.c_str(), connectedClients.length(), 0);

        return client;
    }

    void RemoveClient(Client client)
    {
        clients.erase(client.id);
        cout << "Client disconnected: " << "[" << client.id << "] " << client.name << endl;

        // broadcast disconnection message
        // Format: "(1) Client1 disconnected."
        string msg = "(" + to_string(client.id) + ") " + client.name + " disconnected.";
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

        Client client = AddClient(csocket, name);

        while (true)
        {
            bytesReceived = recv(client.clientSocket, buffer, sizeof(buffer), 0);

            if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0)
            {
                break;
            }

            string message(buffer, bytesReceived);

            // Check if message is PAINT: x,y,color,brushSize
            if (message.find("PAINT:") == 0)
            {
                Broadcast(client, message);
                continue;
            }

            // Check if message is POINTS: points
            if (message.find("POINTS:") == 0)
            {
                int points = stoi(message.substr(8));

                cout << "Giving client (" << client.id << ") " << client.name << " " << points << " points." << endl;
                for (auto &otherClient : clients)
                {
                    if (otherClient.first == client.id)
                    {
                        otherClient.second.points += points;
                    }
                }

                // broadcast points message
                // Format: "(id) points: points."
                string msg = "(" + to_string(client.id) + ") " + " points: " + to_string(points) + ".";
                Broadcast(client, msg);

                continue;
            }

            // Check if message is ANSWER: word
            if (message.find("ANSWER:") == 0)
            {
                // Set chosen word to the received word
                chosenWord = message.substr(7);
                cout << "Chosen word:" << chosenWord << endl;

                // broadcast answer message
                // Format "ANSWER: word"
                Broadcast(client, message);

                continue;
            }

            // Check if round is over
            // Format: "ROUND_OVER"
            if (message.find("ROUND_OVER") == 0)
            {
                chosenWord = "";
                // // broadcast round over message
                // Broadcast(client, message);

                // Set painter to the next client after the current painter
                bool found = false;
                for (auto &otherClient : clients)
                {
                    if (found)
                    {
                        painterID = otherClient.first;
                        break;
                    }

                    if (otherClient.first == painterID)
                    {
                        found = true;
                    }
                }

                // Send the new painter to all clients
                // Format: "PAINTER: id"
                stringstream ss;
                ss << "PAINTER: " << painterID;
                string painter = ss.str();
                Broadcast(client, painter);

                continue;
            }

            // Check start game
            // Format: "START_GAME"
            if (message.find("START_GAME") == 0)
            {
                gameStarted = true;
                Broadcast(client, message);
                continue;
            }

            // Regular message
            // Format: "(ID)[name]: message"
            cout << "[" << client.id << "] " << client.name << ": " << message << endl;

            string msg = "(" + to_string(client.id) + ") [" + client.name + "]: " + message;
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

    Server server;

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
