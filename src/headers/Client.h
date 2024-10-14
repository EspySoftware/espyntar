#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <regex>

// Function collision with raylib
#define Rectangle WIN_Rectangle
#define CloseWindow WIN_CloseWindow
#define ShowCursor WIN_ShowCursor
#define LoadImage WIN_LoadImage
#define DrawText WIN_DrawText
#define DrawTextEx WIN_DrawTextEx
#define PlaySound WIN_PlaySound

#include <WinSock2.h>
#include <WS2tcpip.h>

#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#undef LoadImage
#undef DrawText
#undef DrawTextEx
#undef PlaySound

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::make_shared;
using std::regex;
using std::regex_match;
using std::regex_search;
using std::shared_ptr;
using std::smatch;
using std::stoi;
using std::string;
using std::stringstream;
using std::to_string;
using std::vector;

class OtherClient
{
public:
    int id;
    string name;
    int points;
    bool guessedCorrectly = false;

    OtherClient(int id, string name, int points) { this->id = id, this->name = name, this->points = points; }
};

class PaintMessage
{
public:
    int x;
    int y;
    int color;
    float size;
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
    vector<PaintMessage> paintMessages;
    vector<OtherClient> connectedClients;
    string chosenWord;
    bool round_over = false;

    int adminID = -1;
    int painterID = -1;
    bool guessed = false;

    Client() = default;                                                                                           // Constructor por defecto
    Client(int id, string name, SOCKET socket) { this->id = id, this->name = name, this->clientSocket = socket; } // Constructor para el servidor
    Client(string address, int port = 12345, string name = "Fulanito");                                           // Constructor para el cliente
    ~Client();

    void Send();
    void Send(string message);
    void Receive();

    vector<string> getMessages() { return messages; }
    vector<PaintMessage> getPaintMessages() { return paintMessages; }

    void AddPoints(int points);
    
    void Disconnect();
};

bool InitWinsock();