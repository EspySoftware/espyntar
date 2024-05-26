#pragma once
#include <iostream>
#include <string>

#define Rectangle WIN_Rectangle
#define CloseWindow WIN_CloseWindow
#define ShowCursor WIN_ShowCursor

#include <WinSock2.h>
#include <WS2tcpip.h>

#undef Rectangle
#undef CloseWindow
#undef ShowCursor

using std::cout;
using std::endl;
using std::stoi;
using std::string;

class ChatClient
{
public:
    int id;
    string name;
    SOCKET clientSocket;
    sockaddr_in serveraddr;

    ChatClient(string address, int port = 12345, string name = "Fulanito");
    ~ChatClient();
    void Send(string message);
    string Receive();
};

bool InitWinsock();