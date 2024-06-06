#pragma once
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include "./raygui.h"
#include "./ColorPalette.h"
#include "./Canvas.h"
#include "./Painter.h"
#include "./ChatClient.h"
#include <thread>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::thread;

#define screenWidth 1160
#define screenHeight 760

ChatClient *connectToServer(string ip, string name, int port, thread *senderThread, thread *receiverThread)
{
    cout << "Trying to connect to server " << ip << " with name " << name << endl;
    ChatClient *client = new ChatClient(ip, port, name);

    if (client->clientSocket == INVALID_SOCKET || client->clientSocket == SOCKET_ERROR)
    {
        cout << "Failed to create socket. Error code: " << WSAGetLastError() << endl;
        delete client;
        return nullptr;
    }

    *senderThread = thread([client]()
                           { client->Send(); });

    *receiverThread = thread([client]()
                             { client->Receive(); });

    // senderThread->join();
    // receiverThread->join();

    return client;
}

typedef enum // estructura logica del juego
{
    START,
    GAME,
    EXIT
} GameScene;

class Screen // clase para manejar las pantallas del juego
{
public:
    GameScene scene;
    Texture2D background;
    ChatClient client;

    Screen()
    {
        this->scene = START;
        this->background = LoadTexture("../assets/background.png");
    }

    ~Screen()
    {
        UnloadTexture(this->background);
    }
};

void startGUI(Screen *screen, ChatClient &client, thread *senderThread, thread *receiverThread)
{
    static char name[10] = "Fulanito";
    static char ip[20] = "";
    static bool nameFocus = true;
    static bool ipFocus = false;

    // Cicle focus
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB))
    {
        nameFocus = !nameFocus;
        ipFocus = !ipFocus;
    }

    GuiLabel({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f - 135.0f, 200.0f, 50.0f}, "Name:");
    if (GuiTextBox({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f - 100.0f, 200.0f, 50.0f}, name, 20, nameFocus))
    {
        nameFocus = true;
        ipFocus = false;
    }

    GuiLabel({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f - 35.0f, 200.0f, 50.0f}, "IP:");
    if (GuiTextBox({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f, 200.0f, 50.0f}, ip, 20, ipFocus))
    {
        ipFocus = true;
        nameFocus = false;
    }

    if (GuiButton({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f + 75.0f, 200.0f, 50.0f}, "PLAY"))
    {
        client = *connectToServer(ip, name, 12345, senderThread, receiverThread);

        if (client.clientSocket != INVALID_SOCKET || client.clientSocket != SOCKET_ERROR)
        {
            screen->scene = GAME;
        }
    }

    if (GuiButton({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f + 150.0f, 200.0f, 50.0f}, "EXIT"))
    {
        screen->scene = EXIT;
    }
}

void drawStart(Screen *screen, ChatClient &client, thread *senderThread, thread *receiverThread)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(screen->background, 0, 0, WHITE);

    startGUI(screen, client, senderThread, receiverThread);
    EndDrawing();
}
