#include <string>
#include <thread>
#include <memory>

#include "./headers/ColorPalette.h"
#include "./headers/Canvas.h"
#include "./headers/Painter.h"
#include "./headers/Client.h"
#include "./headers/Inicio.h"
#include "./headers/Game.h"

using std::make_shared;
using std::shared_ptr;
using std::string;
using std::thread;

void PlayGame(shared_ptr<Client> client, thread *senderThread, thread *receiverThread)
{
    // srand(time(NULL));
    SetTraceLogLevel(LOG_NONE); // Disable raylib logging
    InitWindow(screenWidth, screenHeight, "Espyntar");
    InitAudioDevice();
    Texture2D icon = LoadTexture("../assets/logo.png");
    Image icon2 = LoadImageFromTexture(icon);
    bool pause = true;
    UnloadTexture(icon);
    SetWindowIcon(icon2);
    Texture2D espyciales = LoadTexture("../assets/espyciales.png");
    Music music = LoadMusicStream("../assets/victorySound.mp3"); // Asegúrate de que el archivo de música esté en el directorio correcto
    Music musicGame = LoadMusicStream("../assets/gameSound.mp3");
    PlayMusicStream(music);
    PlayMusicStream(musicGame);
    Texture2D espy = LoadTexture("../assets/espy_peke.png");
    Texture2D clock = LoadTexture("../assets/reloj.png");
    Texture2D bgGame = LoadTexture("../assets/backgroundGame.png");
    SetTargetFPS(60);
    Screen screen;

    while (!WindowShouldClose())
    {
        switch (screen.scene)
        {
        case START: // ventana de inicio
            drawStart(&screen, client, senderThread, receiverThread,&musicGame);
            break;
        case GAME: // ventana de juego
            drawGame(&screen, client, &espy, &clock, &bgGame,&musicGame);
            break;
        case WINNER:
            drawWinner(client, &bgGame, &espyciales,&music);
            break;
        case EXIT: // cerrar juego
            CloseWindow();
            break;
        }
    }
    UnloadMusicStream(music);
    UnloadMusicStream(musicGame);
    UnloadTexture(espy);
    UnloadImage(icon2);
    UnloadTexture(clock);
    UnloadTexture(bgGame);
    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    if (!InitWinsock())
    {
        cout << "Failed to initialize Winsock." << endl;
        return 1;
    }

    shared_ptr<Client> client;

    thread senderThread;
    thread receiverThread;

    PlayGame(client, &senderThread, &receiverThread);

    return 0;
}