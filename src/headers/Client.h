#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <regex>

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
using std::regex;
using std::shared_ptr;
using std::smatch;
using std::stoi;
using std::string;
using std::stringstream;
using std::vector;

class OtherClient
{
public:
    int id;
    string name;
    int points;

    OtherClient(int id, string name, int points) { this->id = id, this->name = name, this->points = points; }
};

class Client
{
public:
    int id = -1;
    string name;
    int points = 0;

    SOCKET clientSocket;
    sockaddr_in serverAddress;

    vector<string> messages;
    vector<OtherClient> connectedClients;

    Client() = default;                                                                                           // Constructor por defecto
    Client(int id, string name, SOCKET socket) { this->id = id, this->name = name, this->clientSocket = socket; } // Constructor para el servidor
    Client(string address, int port = 12345, string name = "Fulanito");                                           // Constructor para el cliente
    ~Client();
    void Send();
    void Send(string message);
    void Receive();
    vector<string> getMessages();

    void Disconnect();
};

bool InitWinsock();