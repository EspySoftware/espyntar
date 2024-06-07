#pragma once
#include "./Words.h"
#include "./Inicio.h"
using std::vector;

// Tools
enum Tool
{
    BRUSH,
    BUCKET,
    ERASER
};

void drawChat(shared_ptr<ChatClient> &client)
{
    Font font = GetFontDefault();
    static char message[18] = {0};

    // Get the chat messages from the client
    vector<string> messages = client->getMessages();

    // Draw the chat box (right side of the screen)
    DrawRectangle(GetScreenWidth() - 215, 150, 205, GetScreenHeight() - 205, WHITE);

    // Draw messages in reverse order at the bottom of the chat box
    for (int i = 0; i < messages.size(); i++)
    {
        DrawTextPro(font, messages[messages.size() - i - 1].c_str(), {(float)GetScreenWidth() - 205, (float)GetScreenHeight() - 20 * (i + 1) - 85}, {0, 0}, 0, 14, 4, BLACK);
    }

    // Draw the chat input box
    GuiTextBox({(float)GetScreenWidth() - 215, (float)GetScreenHeight() - 75, 205, 35}, message, 18, true);
}

void drawGame(Screen *screen, shared_ptr<ChatClient> &client)
{
    // Tools
    enum Tool
    {
        BRUSH,
        BUCKET,
        ERASER
    };

    static bool initialized = false;
    static ColorPalette *palette;
    static Canvas *canvas;
    static Painter *painter;
    static Tool currentTool = BRUSH;
    int colorIndex, originalColor;

    // Words
    static Words word;
    vector<string> words;
    static string word1 = word.GetRandomWord();
    static string word2 = word.GetRandomWord();
    static string word3 = word.GetRandomWord();
    static bool chosen = false;

    // Make them mutables
    static char mutableWord1[15];
    strncpy(mutableWord1, word1.c_str(), sizeof(mutableWord1) - 1);
    mutableWord1[sizeof(mutableWord1) - 1] = '\0';

    static char mutableWord2[15];
    strncpy(mutableWord2, word2.c_str(), sizeof(mutableWord2) - 1);
    mutableWord2[sizeof(mutableWord2) - 1] = '\0';

    static char mutableWord3[15];
    strncpy(mutableWord3, word3.c_str(), sizeof(mutableWord3) - 1);
    mutableWord3[sizeof(mutableWord3) - 1] = '\0';

    if (!initialized)
    {
        palette = new ColorPalette();
        canvas = new Canvas(700, 560, *palette);
        painter = new Painter(*palette, *canvas);
        initialized = true;
    }

    Vector2 position = GetMousePosition();

    float mouseWheelMove = GetMouseWheelMove();
    painter->SetBrushSize(mouseWheelMove);

    // Switch tool with SPACE key
    if (IsKeyPressed(KEY_SPACE))
    {
        currentTool = (currentTool == BRUSH) ? BUCKET : BRUSH;
    }

    // Paint or fill if the left mouse button is pressed
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        // Check if a color in the palette is clicked
        colorIndex = canvas->CheckPaletteClick(*palette);
        if (colorIndex >= 0)
        {
            painter->SetColor(colorIndex);
            originalColor = colorIndex;
        }
        else
        {
            if (currentTool == BRUSH)
            {
                painter->Paint(position);
            }
            else if (currentTool == BUCKET)
            {
                painter->Fill(position);
            }
        }
    }
    else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
    {
        painter->SetColor(0); // Set color to white for erasing
        painter->Paint(position);
        painter->SetColor(originalColor); // Restore the original color
    }
    else
    {
        painter->ResetLastPosition();
    }

    // Draw
    Color color_bg = {0, 156, 35, 255};
    BeginDrawing();

    ClearBackground(color_bg);
    // header
    DrawRectangle(30.0f, 20.0f, GetScreenWidth() - 60.0f, 100.0f, {122, 236, 104, 255});

    // Canvas
    Rectangle rec = {0, 0, (float)canvas->GetTarget().texture.width, (float)-canvas->GetTarget().texture.height};
    Vector2 canvasPosition;
    canvasPosition.x = GetScreenWidth() / 2.0f - canvas->GetTarget().texture.width / 2.0f;
    canvasPosition.y = GetScreenHeight() / 2.0f - canvas->GetTarget().texture.height / 2.0f + 50.0f;

    DrawTextureRec(canvas->GetTarget().texture, rec, canvasPosition, WHITE);

    // Brush outline
    if (currentTool == BRUSH)
    {
        DrawCircleLines(GetMouseX(), GetMouseY(), painter->GetBrushSize(), painter->GetColor());
    }
    else if (currentTool == BUCKET)
    {
        DrawCircleLines(GetMouseX(), GetMouseY(), 10, painter->GetColor()); // Small circle for bucket tool indicator
    }

    // Palette
    canvas->DrawPalette(*palette);

    // Draw chat
    drawChat(client);

    // Boton palabras
    if (!chosen)
    {
        if (GuiTextBox({(GetScreenWidth() / 2.0f) - 220, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, mutableWord1, 20, true))
        {
            words.push_back(word1);
            chosen = true;
        }
        if (GuiTextBox({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, mutableWord2, 20, true))
        {
            words.push_back(word2);
            chosen = true;
        }
        if (GuiTextBox({(GetScreenWidth() / 2.0f) + 100, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, mutableWord3, 20, true))
        {
            words.push_back(word3);
            chosen = true;
        }
    }
    EndDrawing();
}
