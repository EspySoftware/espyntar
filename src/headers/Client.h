#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <regex>
#include <unordered_map>
#include "../client/MessageHandler.h"

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
using std::unordered_map;
using std::shared_ptr;

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

struct Point {
    int x;
    int y;
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
    unordered_map<string, shared_ptr<MessageHandler>> handlers;
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

    // Función para interpolar puntos entre dos puntos dados
    std::vector<Point> interpolatePoints(Point p1, Point p2, int brushSize) {
        std::vector<Point> interpolatedPoints;
        int distance = std::hypot(p2.x - p1.x, p2.y - p1.y);
        int steps = distance / brushSize;

        for (int i = 0; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            int x = p1.x + t * (p2.x - p1.x);
            int y = p1.y + t * (p2.y - p1.y);
            interpolatedPoints.push_back({x, y});
        }

        return interpolatedPoints;
    }

    void handleMessage(const std::string& message);
};

bool InitWinsock();