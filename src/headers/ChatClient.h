#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define Rectangle WIN_Rectangle
#define CloseWindow WIN_CloseWindow
#define ShowCursor WIN_ShowCursor

#include <WinSock2.h>
#include <WS2tcpip.h>

#undef Rectangle
#undef CloseWindow
#undef ShowCursor

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::make_shared;
using std::shared_ptr;
using std::stoi;
using std::string;
using std::vector;

class ChatClient
{
public:
    int id = -1;
    string name;
    SOCKET clientSocket;
    sockaddr_in serverAddress;
    vector<string> messages;

    ChatClient() = default;                                                                                           // Constructor por defecto
    ChatClient(int id, string name, SOCKET socket) { this->id = id, this->name = name, this->clientSocket = socket; } // Constructor para el servidor
    ChatClient(string address, int port = 12345, string name = "Fulanito");                                           // Constructor para el cliente
    ~ChatClient();
    void Send();
    void Receive();
    vector<string> getMessages();

    void Disconnect();
};

bool InitWinsock();