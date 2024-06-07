#pragma once
#include "../headers/Inicio.h"

using std::vector;

// Tools
enum Tool
{
    BRUSH,
    BUCKET,
    ERASER
};

class Game
{
public:
    // bool initialized = false;
    // ColorPalette *palette;
    // Canvas *canvas;
    // Painter *painter;
    // Tool currentTool = BRUSH;
    // int colorIndex, originalColor;
    // GameScene scene;
    Texture2D background;
    

    Game()
    {
        // this->scene = GAME;
        this->background = LoadTexture("../assets/espy_peke.png");
        // this->palette = new ColorPalette();
        // this->canvas = new Canvas(700, 560, *palette);
        // this->painter = new Painter(*palette, *canvas);
        // this->initialized = true;
    }
    ~Game()
    {
        UnloadTexture(this->background);
    }

    void drawGame(Screen *screen, shared_ptr<ChatClient> &client);
};




// void drawGame(Screen *screen, shared_ptr<ChatClient> &client)
// {
//     // static bool initialized = false;
//     // static ColorPalette *palette;
//     // static Canvas *canvas;
//     // static Painter *painter;
//     // static Tool currentTool = BRUSH;
//     // int colorIndex, originalColor;

//     // if (!initialized)
//     // {
//     //     palette = new ColorPalette();
//     //     canvas = new Canvas(700, 560, *palette);
//     //     painter = new Painter(*palette, *canvas);
//     //     initialized = true;
//     // }

//     Vector2 position = GetMousePosition();

//     float mouseWheelMove = GetMouseWheelMove();
//     painter->SetBrushSize(mouseWheelMove);

//     // Switch tool with SPACE key
//     if (IsKeyPressed(KEY_SPACE))
//     {
//         currentTool = (currentTool == BRUSH) ? BUCKET : BRUSH;
//     }

//     // Paint or fill if the left mouse button is pressed
//     if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
//     {
//         // Check if a color in the palette is clicked
//         colorIndex = canvas->CheckPaletteClick(*palette);
//         if (colorIndex >= 0)
//         {
//             painter->SetColor(colorIndex);
//             originalColor = colorIndex;
//         }
//         else
//         {
//             if (currentTool == BRUSH)
//             {
//                 painter->Paint(position);
//             }
//             else if (currentTool == BUCKET)
//             {
//                 painter->Fill(position);
//             }
//         }
//     }
//     else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
//     {
//         painter->SetColor(0); // Set color to white for erasing
//         painter->Paint(position);
//         painter->SetColor(originalColor); // Restore the original color
//     }
//     else
//     {
//         painter->ResetLastPosition();
//     }

//     // Draw
//     Color color_bg = {0, 156, 35, 255};
//     BeginDrawing();
//     ClearBackground(color_bg);
//     // header
//     DrawRectangle(10.0f, 50.0f, GetScreenWidth() - 20.0f, 100.0f, {122, 236, 104, 255});

//     // Canvas
//     Rectangle rec = {0, 0, (float)canvas->GetTarget().texture.width, (float)-canvas->GetTarget().texture.height};
//     Vector2 canvasPosition;
//     canvasPosition.x = GetScreenWidth() / 2.0f - canvas->GetTarget().texture.width / 2.0f;
//     canvasPosition.y = GetScreenHeight() / 2.0f - canvas->GetTarget().texture.height / 2.0f + 70.0f;

//     DrawTextureRec(canvas->GetTarget().texture, rec, canvasPosition, WHITE);

//     // Brush outline
//     if (currentTool == BRUSH)
//     {
//         DrawCircleLines(GetMouseX(), GetMouseY(), painter->GetBrushSize(), painter->GetColor());
//     }
//     else if (currentTool == BUCKET)
//     {
//         DrawCircleLines(GetMouseX(), GetMouseY(), 10, painter->GetColor()); // Small circle for bucket tool indicator
//     }

//     // Palette
//     canvas->DrawPalette(*palette);

//     // Draw chat
//     drawChat(client);

//     EndDrawing();
// }
