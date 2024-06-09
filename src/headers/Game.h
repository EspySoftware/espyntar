#pragma once
#include "./Games.h"
#include "./Inicio.h"
#include "./Partida.h"
using std::array;
using std::vector;
#define FRAMES 144

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
    vector<OtherClient> drawConnectedClients = client->connectedClients;
    // Draw connected clients box (left)
    DrawRectangle(10, 170, 200, GetScreenHeight() - 200, WHITE);
    DrawTextPro(font, "JUGADORES", {50, 175}, {0, 0}, 0, 20, 2, BLACK);

    for (int i = 0; i < drawConnectedClients.size(); i++)
    {
        float yOffset = 200.0f + 60 * i;
        Rectangle recPlayer = {15, yOffset, 190, 70};

        // Draw box for each client
        if (drawConnectedClients[i].id == client->painterID)
        {
            // Draw a different color for the painter
            DrawRectangleRec(recPlayer, {215, 182, 15, 255});
        }
        else
        {
            // Draw a different color for the current client
            DrawRectangleRec(recPlayer, {122, 236, 104, 255});
        }

        DrawRectangleLinesEx(recPlayer, 1, {87, 179, 72, 255});

        // Draw the client name (centered in the box)
        Vector2 textSize = MeasureTextEx(font, drawConnectedClients[i].name.c_str(), 15, 2);
        float nameX = recPlayer.x + (recPlayer.width - textSize.x) / 2;
        float nameY = recPlayer.y + (recPlayer.height - textSize.y) / 4;

        DrawTextPro(font, drawConnectedClients[i].name.c_str(), {nameX, nameY}, {0, 0}, 0, 15, 2, DARKGRAY);

        // Draw the client points (centered below the name)
        std::string pointsStr = std::to_string(drawConnectedClients[i].points);
        textSize = MeasureTextEx(font, pointsStr.c_str(), 15, 2);
        float pointsX = recPlayer.x + (recPlayer.width - textSize.x) / 2;
        float pointsY = nameY + textSize.y + 5;

        DrawTextPro(font, pointsStr.c_str(), {pointsX, pointsY}, {0, 0}, 0, 15, 2, DARKGRAY);

        // Draw the client id (left side of the box)
        std::string idStr = "#" + std::to_string(drawConnectedClients[i].id);
        textSize = MeasureTextEx(font, idStr.c_str(), 15, 2);
        float idX = recPlayer.x + 5;
        float idY = nameY + textSize.y - 5;

        DrawTextPro(font, idStr.c_str(), {idX, idY}, {0, 0}, 0, 15, 2, BLACK);
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
    // Format: "(id) [name]: message"
    regex r("\\((\\d+)\\)\\s+\\[(\\w+)\\]:\\s+(.*)");
    smatch match;

    for (int i = 0; i < maxMessages; i++)
    {
        if (regex_search(messages[messages.size() - i - 1], match, r) && match.size() > 3)
        {
            int id = stoi(match.str(1));
            string name = match.str(2);
            string msg = match.str(3);

            // Filter chosenWord messages from the chat
            // Lowercased message to lowercased chosenWord
            string lowerMsg = msg;
            std::transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(), ::tolower);
            string lowerChosenWord = client->chosenWord;
            std::transform(lowerChosenWord.begin(), lowerChosenWord.end(), lowerChosenWord.begin(), ::tolower);

            if (lowerMsg == lowerChosenWord)
            {
                msg = name + " guessed!";
                client->messages[messages.size() - i - 1] = msg;
            }
            else
            {
                // Draw the message
                name = "[" + name + "]: ";
                DrawTextPro(font, name.c_str(), {(float)GetScreenWidth() - 210, (float)GetScreenHeight() - 20 * (i + 1) - 75}, {0, 0}, 0, 12, 2, BLACK);
                DrawTextPro(font, msg.c_str(), {(float)GetScreenWidth() - 210 + MeasureText(name.c_str(), 14), (float)GetScreenHeight() - 20 * (i + 1) - 75}, {0, 0}, 0, 12, 2, DARKGRAY);
            }
        }
        else
        {
            // Draw past guessed messages
            if (messages[messages.size() - i - 1].find("guessed!") != string::npos)
            {
                DrawTextPro(font, messages[messages.size() - i - 1].c_str(), {(float)GetScreenWidth() - 210, (float)GetScreenHeight() - 20 * (i + 1) - 75}, {0, 0}, 0, 12, 2, DARKGREEN);
            }
            else
            {
                // Draw the message
                DrawTextPro(font, messages[messages.size() - i - 1].c_str(), {(float)GetScreenWidth() - 210, (float)GetScreenHeight() - 20 * (i + 1) - 75}, {0, 0}, 0, 12, 2, GRAY);
            }
        }
    }

    // Draw the chat input box
    if (!client->guessed || !client->painterID == client->id)
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

void drawPaintMessages(shared_ptr<Client> &client, Painter *painter)
{
    vector<PaintMessage> paintMessages = client->getPaintMessages();

    for (int i = 0; i < paintMessages.size(); i++)
    {
        Vector2 position = {(float)paintMessages[i].x, (float)paintMessages[i].y};
        painter->Paint(position, paintMessages[i].color, paintMessages[i].size);
    }

    // Clear the paint messages
    client->paintMessages.clear();
}

void drawGame(Screen *screen, shared_ptr<Client> &client, Texture2D *espy, Texture2D *clock, Texture2D *bgGame)
{
    // Tools
    enum Tool
    {
        BRUSH,
        ERASER
    };
    DrawTexture(*bgGame, 0, 0, WHITE);

    // Objects
    ColorPalette *palette;
    Canvas *canvas;
    Painter *painter;
    Games *game;
    Partida *partida;

    // Variables
    static bool initialized = false;
    bool config = false;
    static Tool currentTool = BRUSH;
    static int colorIndex;
    static bool isGuesser = false;

    static int CUTE = 1;

    if (!initialized)
    {
        // Compare client->painterID with client->id
        if (client->painterID == client->id)
        {
            isGuesser = false;
        }
        else
        {
            isGuesser = true;
        }

        palette = new ColorPalette();
        canvas = new Canvas(700, 560, *palette);
        painter = new Painter(*palette, *canvas);
        game = new Games(*painter, *canvas, *palette, isGuesser);
        partida = new Partida(*game, client);
        initialized = true;
    }

    if (CUTE)
    {
        Vector2 position = GetMousePosition();

        float mouseWheelMove = GetMouseWheelMove();
        painter->SetBrushSize(mouseWheelMove);

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
                    painter->Paint(position, client);
                }
            }
        }
        else if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON))
        {
            painter->SetColor(0); // Set color to white for erasing
            painter->Paint(position, client);
            painter->SetColor(colorIndex); // Restore the original color
        }
        else
        {
            painter->ResetLastPosition();
        }

        // Draw
        Color color_bg = {0, 156, 35, 255};
        BeginDrawing();
        // ClearBackground(color_bg);

        // Header

        buttons(10.0f, 50.0f, GetScreenWidth() - 20.0f, 100.0f, "muchotexto", {122, 236, 104, 255});
        DrawRectangle(10.0f, 60.0f, GetScreenWidth() - 20.0f, 80.0f, {122, 236, 104, 255});
        DrawTexture(*clock, 20, 60, WHITE);
        DrawTexture(*(espy), GetScreenWidth() / 2.0f - ((espy->width) / 2.0f), 5, WHITE);

        buttons(1045.0f, 100.0f - 25.0f, 70.0f, 50.0f, "SALIR", {215, 182, 15, 255});
        if (GuiButton({1045.0f, 100.0f - 25.0f, 70.0f, 50.0f}, "SALIR"))
        {
            screen->scene = EXIT;
        }

        // Canvas
        Rectangle rec = {0, 0, (float)canvas->GetTarget().texture.width, (float)-canvas->GetTarget().texture.height};
        Vector2 canvasPosition;
        canvasPosition.x = GetScreenWidth() / 2.0f - canvas->GetTarget().texture.width / 2.0f;
        canvasPosition.y = GetScreenHeight() / 2.0f - canvas->GetTarget().texture.height / 2.0f + 70.0f;

        DrawTextureRec(canvas->GetTarget().texture, rec, canvasPosition, WHITE);

        // Brush outline
        if (!game->GetIsGuesser())
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
    }

    // Draw chat
    drawChat(client);

    // Draw paint messages
    drawPaintMessages(client, painter);

    // Draw game
    partida->Ronda(client);
    partida->DrawRounds();

    // Draw connected clients
    drawConnectedClients(client);

    EndDrawing();
}
