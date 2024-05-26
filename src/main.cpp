#include <iostream>
#include <string>
#include <thread>

#include "./headers/ColorPalette.h"
#include "./headers/Canvas.h"
#include "./headers/Painter.h"
#include "./headers/ChatClient.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::thread;

void Game()
{
    // Juego
    const int screenWidth = 800;
    const int screenHeight = 450;

    // SetTraceLogLevel(LOG_NONE); // Disable raylib logging
    InitWindow(screenWidth, screenHeight, "Espyntar");

    ColorPalette palette;
    Canvas canvas(screenWidth, screenHeight, palette);
    Painter painter(palette, canvas);

    SetTargetFPS(144);

    while (!WindowShouldClose())
    {
        Vector2 position = GetMousePosition();

        float mouseWheelMove = GetMouseWheelMove();
        painter.SetBrushSize(mouseWheelMove);

        // Paint if the left mouse button is pressed
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            // Check if a color in the palette is clicked
            int colorIndex = canvas.CheckPaletteClick(palette);
            if (colorIndex >= 0)
            {
                painter.SetColor(colorIndex);
            }
            else
            {
                painter.Paint(position);
            }
        }
        else
        {
            painter.ResetLastPosition();
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Canvas
        Rectangle rec = {0.0f, 0.0f, (float)canvas.GetTarget().texture.width, (float)-canvas.GetTarget().texture.height};
        Vector2 vec = {0.0f, 0.0f};
        DrawTextureRec(canvas.GetTarget().texture, rec, vec, WHITE);

        // Brush
        DrawCircleLines(GetMouseX(), GetMouseY(), painter.GetBrushSize(), painter.GetColor());

        // Palette
        canvas.DrawPalette(palette);
        DrawFPS(GetScreenWidth() - 95, 10);
        EndDrawing();
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