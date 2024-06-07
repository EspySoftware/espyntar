#include <iostream>
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

void PlayGame(shared_ptr<ChatClient> client, thread *senderThread, thread *receiverThread)
{
    // SetTraceLogLevel(LOG_NONE); // Disable raylib logging
    InitWindow(screenWidth, screenHeight, "Espyntar");
    Texture2D icon = LoadTexture("../assets/logo.png");
    Image icon2 = LoadImageFromTexture(icon);
    UnloadTexture(icon);
    SetWindowIcon(icon2);

    // Tools
    enum Tool
    {
        BRUSH,
        BUCKET
    };

    ColorPalette palette;
    Canvas canvas(screenWidth, screenHeight, palette);
    Painter painter(palette, canvas);
    Tool currentTool = BRUSH;

    SetTargetFPS(144);
    Screen screen;
    while (!WindowShouldClose())
    {
        Vector2 position = GetMousePosition();

        float mouseWheelMove = GetMouseWheelMove();
        painter.SetBrushSize(mouseWheelMove);

        // Switch tool with SPACE key
        if (IsKeyPressed(KEY_SPACE))
        {
            currentTool = (currentTool == BRUSH) ? BUCKET : BRUSH;
        }

        // Paint or fill if the left mouse button is pressed
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
                if (currentTool == BRUSH)
                {
                    painter.Paint(position);
                }
                else if (currentTool == BUCKET)
                {
                    painter.Fill(position);
                }
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

        // Brush outline
        if (currentTool == BRUSH)
        {
            DrawCircleLines(GetMouseX(), GetMouseY(), painter.GetBrushSize(), painter.GetColor());
        }
        else if (currentTool == BUCKET)
        {
            DrawCircleLines(GetMouseX(), GetMouseY(), 10, painter.GetColor()); // Small circle for bucket tool indicator
        }

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

    shared_ptr<ChatClient> client;

    thread senderThread;
    thread receiverThread;

    PlayGame(client, &senderThread, &receiverThread);

    return 0;
}