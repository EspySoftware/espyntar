#ifndef SOCKETFACTORY_H
#define SOCKETFACTORY_H

#include "./Client.h"

class SocketFactory
{
public:
    static SOCKET CreateSocket()
    {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == INVALID_SOCKET)
        {
            std::cout << "Failed to create socket." << std::endl;
        }
        return sock;
    }

    static bool ConnectSocket(SOCKET sock, const std::string& address, int port, sockaddr_in& serverAddress)
    {
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        inet_pton(AF_INET, address.c_str(), &(serverAddress.sin_addr));

        if (connect(sock, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
        {
            std::cout << "Failed to connect to the server." << std::endl;
            std::cout << "Error code: " << WSAGetLastError() << std::endl;
            closesocket(sock);
            WSACleanup();
            return false;
        }

        std::cout << "Connected to the server at " << address << ":" << port << std::endl;
        return true;
    }
};

#endif // SOCKETFACTORY_H