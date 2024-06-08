#pragma once
#include "./Words.h"
#include "./Inicio.h"
#include "../headers/Inicio.h"
using std::array;
using std::vector;

// Tools
enum Tool
{
    BRUSH,
    BUCKET,
    ERASER
};
void buttons(float x, float y, float widght, float height, char text[])
{
    Rectangle buttonRect = {x, y, widght, height};
    Rectangle buttonRect2 = {x - 2.0f, y - 2.0f, widght + 5.0f, height + 5.0f};

    DrawRectangleRounded(buttonRect2, 0.3f, 6, {215, 182, 15, 255});
    // Dibuja el texto del botón
    if (GuiButton(buttonRect, text))
    {
        // Acción del botón
    }
}
void drawConnectedClients(shared_ptr<ChatClient> &client)
{
    Font font = GetFontDefault();
    vector<string> connectedClients = client->connectedClients;

    // Draw connected clients box (left)
    DrawRectangle(10, 170, 200, GetScreenHeight() - 200, WHITE);
    DrawTextPro(font, "Connected Clients", {15, 175}, {0, 0}, 0, 20, 2, BLACK);

    for (int i = 0; i < connectedClients.size(); i++)
    {
        // Draw box for each client
        DrawRectangle(15, 200.0f + 20 * i, 190, 20, WHITE);
        DrawRectangleLines(15, 200.0f + 20 * i, 190, 20, DARKGRAY);

        // Draw the client name
        DrawTextPro(font, connectedClients[i].c_str(), {20, 205.0f + 20 * i}, {0, 0}, 0, 14, 2, BLACK);
    }
}

void drawChat(shared_ptr<ChatClient> &client)
{
    Font font = GetFontDefault();
    static char message[20] = {0};

    // Get the chat messages from the client
    vector<string> messages = client->getMessages();

    // Draw the chat box (right side of the screen)
    DrawRectangle(GetScreenWidth() - 215, 170, 205, GetScreenHeight() - 200, WHITE);

    // Draw messages in reverse order at the bottom of the chat box
    int maxMessages = 25 < messages.size() ? 25 : messages.size();
    for (int i = 0; i < maxMessages; i++)
    {
        string name = messages[messages.size() - i - 1].substr(0, messages[messages.size() - i - 1].find(":") + 1);
        string msg = messages[messages.size() - i - 1].substr(messages[messages.size() - i - 1].find(":") + 1);

        DrawTextPro(font, name.c_str(), {(float)GetScreenWidth() - 210, (float)GetScreenHeight() - 20 * (i + 1) - 75}, {0, 0}, 0, 12, 2, BLACK);
        DrawTextPro(font, msg.c_str(), {(float)GetScreenWidth() - 210 + MeasureText(name.c_str(), 14), (float)GetScreenHeight() - 20 * (i + 1) - 75}, {0, 0}, 0, 12, 2, DARKGRAY);
    }

    // Draw the chat input box
    GuiTextBox({(float)GetScreenWidth() - 215, (float)GetScreenHeight() - 65, 205, 35}, message, 13, true);

    // Usando GuiTextBox no editable
    Color color_chat = {252, 229, 113, 255};
    char text[5] = "CHAT";
    DrawRectangle((float)GetScreenWidth() - 215, (float)GetScreenHeight() - 590, 205, 35, color_chat);
    DrawTextPro(GetFontDefault(), text, {(float)GetScreenWidth() - 130, (float)GetScreenHeight() - 580},
                {0.0f, 0.0f}, 0.0f, 15.0f, 0.0f, BLACK);
    // Send the message when the user presses Enter
    if (IsKeyPressed(KEY_ENTER) && strlen(message) > 0)
    {
        client->Send(message);
        strcpy(message, "");
    }
}

void drawGame(Screen *screen, shared_ptr<ChatClient> &client, Texture2D *espy)
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
    static int colorIndex;
    static int timer = 0;

    // Words
    static Words word;
    static array<string, 3> words = word.GetRandomWords();
    static bool chosen = false;

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
        painter->SetColor(colorIndex); // Restore the original color
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
    DrawRectangle(10.0f, 50.0f, GetScreenWidth() - 20.0f, 100.0f, {122, 236, 104, 255});
    DrawTexture(*(espy), GetScreenWidth() / 2.0f - ((espy->width) / 2.0f), 5, WHITE);

    // Canvas
    Rectangle rec = {0, 0, (float)canvas->GetTarget().texture.width, (float)-canvas->GetTarget().texture.height};
    Vector2 canvasPosition;
    canvasPosition.x = GetScreenWidth() / 2.0f - canvas->GetTarget().texture.width / 2.0f;
    canvasPosition.y = GetScreenHeight() / 2.0f - canvas->GetTarget().texture.height / 2.0f + 70.0f;

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

    // Draw connected clients
    drawConnectedClients(client);

    // Boton palabras
    if (!chosen)
    {
        if (GuiButton({(GetScreenWidth() / 2.0f) - 220, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[0].c_str()))
        {
            word.SetChosenWord(words[0]);
            chosen = true;
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[1].c_str()))
        {

            word.SetChosenWord(words[1]);
            chosen = true;
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) + 100, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[2].c_str()))
        {
            word.SetChosenWord(words[2]);
            chosen = true;
        }
    }

    // Draw the words
    if (chosen)
    {
        DrawTextPro(GetFontDefault(), word.GetChosenWord().c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(word.GetChosenWord().c_str(), 20) / 2), +60}, {0, 0}, 0, 20, 4, BLACK);
        for (int i = 0; i < word.GetChosenWord().size(); i++)
        {
            DrawRectangle((GetScreenWidth() / 2.0f) - (MeasureText(word.GetChosenWord().c_str(), 20)) + i * 30 - 15, 90.0f, 20.0f, 5.0f, BLACK);
        }
    }

    EndDrawing();
}
