#include <string>
#include <thread>

#include "./headers/ColorPalette.h"
#include "./headers/Canvas.h"
#include "./headers/Painter.h"
#include "./headers/ChatClient.h"
#include "./headers/Inicio.h"

using std::string;
using std::thread;

void Game()
{
    // Juego
    GameScene scene = START;

    // SetTraceLogLevel(LOG_NONE); // Disable raylib logging
    InitWindow(screenWidth, screenHeight, "Espyntar");

    SetTargetFPS(144);

    while (!WindowShouldClose())
    {
        switch (scene)
        {
        case START: // ventana de inicio
            drawStart(&scene);
            break;
        case GAME: // ventana de juego 
            drawGame();
            break;
        }
    }

    CloseWindow();
}

int main(void)
{
    if (!InitWinsock())
    {
        cout << "Failed to initialize Winsock." << endl;
        return 1;
    }

    // Conexión al servidor
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

    string name;
    cout << "Ingrese su nombre: ";
    getline(cin, name);
    ChatClient client("127.0.0.1", 12345, name);

    thread senderThread([&client]()
                        { client.Send(); });

    thread receiverThread([&client]()
                          { client.Receive(); });

    thread juegoThread(Game);

    juegoThread.join();
    senderThread.join();
    receiverThread.join();

    return 0;
}