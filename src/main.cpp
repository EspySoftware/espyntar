#include <iostream>
#include <string>

#include "./headers/ColorPalette.h"
#include "./headers/Canvas.h"
#include "./headers/Painter.h"
#include "./headers/ChatClient.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;

int main(void)
{
    if (!InitWinsock())
    {
        cout << "Failed to initialize Winsock." << endl;
        return 1;
    }

    cout << "Espyntar Client" << endl;

    // Conexión al servidor
    string name;
    cout << "Ingrese su nombre: ";
    getline(cin, name);
    ChatClient client("127.0.0.1", 12345, name);

    // Juego
    const int screenWidth = 800;
    const int screenHeight = 450;

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

    return 0;
}