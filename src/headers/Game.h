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

void drawConnectedClients(shared_ptr<Client> &client)
{
    Font font = GetFontDefault();
    vector<OtherClient> connectedClients = client->connectedClients;

    // Draw connected clients box (left)
    DrawRectangle(10, 170, 200, GetScreenHeight() - 200, WHITE);
    DrawTextPro(font, "Connected Clients", {15, 175}, {0, 0}, 0, 20, 2, BLACK);

    for (int i = 0; i < connectedClients.size(); i++)
    {
        // Draw box for each client
        DrawRectangle(15, 200.0f + 40 * i, 190, 20, WHITE);
        DrawRectangleLines(15, 200.0f + 40 * i, 190, 20, DARKGRAY);

        // Draw the client id
        DrawTextPro(font, ("(" + std::to_string(connectedClients[i].id) + ")").c_str(), {20, 200.0f + 20 * i}, {0, 0}, 0, 12, 2, BLACK);

        // Draw the client name (right side)
        DrawTextPro(font, connectedClients[i].name.c_str(), {50, 200.0f + 20 * i}, {0, 0}, 0, 12, 2, DARKGRAY);

        // Draw the client points (bottom)
        DrawTextPro(font, std::to_string(connectedClients[i].points).c_str(), {180, 200.0f + 20 * i}, {0, 0}, 0, 12, 2, DARKGRAY);
    }
}

void drawChat(shared_ptr<Client> &client)
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
                {0.0f, 0.0f}, 0.0f, 15.0f, 2.0f, BLACK);
    // Send the message when the user presses Enter
    if (IsKeyPressed(KEY_ENTER) && strlen(message) > 0)
    {
        client->Send(message);
        strcpy(message, "");
    }
}

void drawGame(Screen *screen, shared_ptr<Client> &client, Texture2D *espy)
{
    // Tools
    enum Tool
    {
        BRUSH,
        ERASER
    };

    static bool initialized = false;
    static ColorPalette *palette;
    static Canvas *canvas;
    static Painter *painter;
    static Words *word;
    static Tool currentTool = BRUSH;
    static int colorIndex;

    if (!initialized)
    {
        palette = new ColorPalette();
        canvas = new Canvas(700, 560, *palette);
        painter = new Painter(*palette, *canvas);
        word = new Words(*painter, *canvas, *palette);
        initialized = true;
    }

    Vector2 position = GetMousePosition();

    float mouseWheelMove = GetMouseWheelMove();
    painter->SetBrushSize(mouseWheelMove);

    // Switch tool with SPACE key
    // if (GuiButton({(GetScreenWidth() / 2.0f) - 220, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, currentTool == BRUSH ? "Eraser" : "Brush"))
    // {
    //     currentTool = (currentTool == BRUSH) ? BRUSH : ERASER;
    // }
    // {
    //     currentTool = (currentTool == BRUSH) ? BRUSH : ERASER;
    // }

    // Paint
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        colorIndex = canvas->CheckPaletteClick(*palette);
        if (colorIndex >= 0)
        {
            painter->SetColor(colorIndex);
        }
        else
        {
            if (currentTool == ERASER)
            {
                painter->Erase(position);
            }
            else if (currentTool == BRUSH)
            {
                painter->Paint(position);
            }
        }
    }
    else
    {
        painter->ResetLastPosition();
    }

    // Draw
    Color color_bg = {0, 156, 35, 255};
    BeginDrawing();
    ClearBackground(color_bg);

    // Header
    DrawRectangle(10.0f, 50.0f, GetScreenWidth() - 20.0f, 100.0f, {122, 236, 104, 255});
    DrawTexture(*(espy), GetScreenWidth() / 2.0f - ((espy->width) / 2.0f), 5, WHITE);
    buttons(1050.0f, 100.0f - 25.0f, 50.0f, 50.0f, "#142#");
    if (GuiButton({1050.0f, 100.0f - 25.0f, 50.0f, 50.0f}, "#142#"))
    {
        screen->scene = CONFIG;
        cout << "Pantalla configuracion" << endl;
    }

    // Canvas
    Rectangle rec = {0, 0, (float)canvas->GetTarget().texture.width, (float)-canvas->GetTarget().texture.height};
    Vector2 canvasPosition;
    canvasPosition.x = GetScreenWidth() / 2.0f - canvas->GetTarget().texture.width / 2.0f;
    canvasPosition.y = GetScreenHeight() / 2.0f - canvas->GetTarget().texture.height / 2.0f + 70.0f;

    DrawTextureRec(canvas->GetTarget().texture, rec, canvasPosition, WHITE);

    // Brush outline
    if (!word->GetisGuesser())
    {
        if (currentTool == BRUSH)
        {
            DrawCircleLines(GetMouseX(), GetMouseY(), painter->GetBrushSize(), painter->GetColor());
        }
        else if (currentTool == ERASER)
        {
            DrawCircleLines(GetMouseX(), GetMouseY(), 10, painter->GetColor()); // Small circle for bucket tool indicator
        }
    }

    // Palette
    canvas->DrawPalette(*palette);

    // Draw chat
    drawChat(client);

    // Draw connected clients
    drawConnectedClients(client);

    // Draw option words
    if (!word->GetChosen())
    {
        word->SetChosenWord();
    }
    if (word->GetChosen())
    {
        word->DrawChosenWord();
    }

    EndDrawing();
}
