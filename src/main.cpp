#include <string>
#include <thread>
#include <memory>

#include "./headers/ColorPalette.h"
#include "./headers/Canvas.h"
#include "./headers/Painter.h"
#include "./headers/ChatClient.h"
#include "./headers/Inicio.h"
#include "./headers/Game.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::thread;

void Game(shared_ptr<ChatClient> client, thread *senderThread, thread *receiverThread)
{
    // SetTraceLogLevel(LOG_NONE); // Disable raylib logging
    InitWindow(screenWidth, screenHeight, "Espyntar");
    Texture2D icon = LoadTexture("../assets/logo.png");
    Image icon2 = LoadImageFromTexture(icon);
    UnloadTexture(icon);
    SetWindowIcon(icon2);

    SetTargetFPS(144);
    Screen screen;
    
    while (!WindowShouldClose())
    {
        {
            switch (screen.scene)
            {
            case START: // ventana de inicio
                drawStart(&screen, client, senderThread, receiverThread);
                break;
            case GAME: // ventana de juego
                drawGame(&screen, client);
                break;
            case EXIT: // cerrar juego
                CloseWindow();
                break;
            }
        }
    }
    UnloadImage(icon2);
    CloseWindow();
}

int main(void)
{
    if (!InitWinsock())
    {
        cout << "Failed to initialize Winsock." << endl;
        return 1;
    }

    shared_ptr<ChatClient> client;

    thread senderThread;
    thread receiverThread;

    Game(client, &senderThread, &receiverThread);

    return 0;
}