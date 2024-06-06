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

ChatClient connectToServer(string ip, string name = "Fulanito", int port = 12345)
{
    cout << "Trying to connect to server " << ip << " with name " << name << endl;
    ChatClient client(ip, port, name);
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
    thread senderThread;
    thread receiverThread;
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

void startGUI(Screen *screen) // GUI de la pantalla de inicio
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
        screen->client = connectToServer(ip, name);

        if (screen->client.clientSocket != INVALID_SOCKET || screen->client.clientSocket != SOCKET_ERROR)
        {
            screen->senderThread = thread([&screen]()
                                          { screen->client.Send(); });

            screen->receiverThread = thread([&screen]()
                                            { screen->client.Receive(); });

            screen->scene = GAME;
        }
    }

    if (GuiButton({GetScreenWidth() / 2.0f - 100.0f, GetScreenHeight() / 2.0f + 150.0f, 200.0f, 50.0f}, "EXIT"))
    {
        screen->scene = EXIT;
    }
}

void drawStart(Screen *screen) // dibuja la pantalla de inicio
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(screen->background, 0, 0, WHITE);

    startGUI(screen);
    EndDrawing();
}
