#pragma once
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include <thread>
#include "./raygui.h"
#include "./ColorPalette.h"
#include "./Canvas.h"
#include "./Painter.h"
#include "./Client.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::thread;

#define screenWidth 1160
#define screenHeight 760

bool connectToServer(shared_ptr<Client> &client, string ip, string name, int port, thread *senderThread, thread *receiverThread)
{
    cout << "Trying to connect to server " << ip << " with name " << name << endl;
    client = make_shared<Client>(ip, port, name);

    if (client->clientSocket == INVALID_SOCKET || client->clientSocket == SOCKET_ERROR)
    {
        cout << "Failed to create socket. Error code: " << WSAGetLastError() << endl;
        return false;
    }

    *senderThread = thread([client]()
                           { client->Send(); });
    *receiverThread = thread([client]()
                             { client->Receive(); });

    return true;
}

typedef enum // estructura logica del juego
{
    START,
    GAME,
    CONFIG,
    EXIT
} GameScene;

class Screen // clase para manejar las pantallas del juego
{
public:
    GameScene scene;
    Texture2D background;
    Texture2D title;

    Screen()
    {
        this->scene = START;
        this->background = LoadTexture("../assets/background.png");
        this->title = LoadTexture("../assets/espy.png");
    }

    ~Screen()
    {
        UnloadTexture(this->background);
        UnloadTexture(this->title);
    }
};

void buttons(float x, float y, float width, float height, const char *text)
{
    Rectangle buttonRect = {x, y, width, height};
    Rectangle buttonRect2 = {x - 2.0f, y - 2.0f, width + 4.0f, height + 4.0f};

    DrawRectangleRounded(buttonRect2, 0.3f, 6, {215, 182, 15, 255});
}

void startGUI(Screen *screen, shared_ptr<Client> &client, thread *senderThread, thread *receiverThread)
{
    Color color_base = {44, 74, 36, 200};
    Color color_names = {252, 229, 113, 255};
    Color color_button = {249, 217, 53, 255};
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(color_base));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_DISABLED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, ColorToInt(color_base)); // Ca

    GuiSetStyle(BUTTON, BORDER_COLOR_DISABLED, ColorToInt({215, 182, 15, 255}));
    GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt({192, 169, 29, 255}));
    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt({215, 182, 15, 255}));
    GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt({215, 182, 15, 255}));

    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(color_button));
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(color_button));
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt({236, 204, 39, 255}));
    GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(BLACK));
    GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(BLACK));
    GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(BLACK));
    static char name[16] = "";
    static char ip[20] = "";
    static bool nameFocus = true;
    static bool ipFocus = false;

    // Cicle focus
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB))
    {
        nameFocus = !nameFocus;
        ipFocus = !ipFocus;
    }

    DrawTextPro(GetFontDefault(), "Nombre:", {1026, 650}, {580, 380}, 0.0f, 20.0f, 4.0f, color_names);
    if (GuiTextBox({(GetScreenWidth() / 2.0f) - 110.0f, GetScreenHeight() / 2.0f - 90.0f, 220.0f, 60.0f}, name, 9, nameFocus))
    {
        nameFocus = true;
        ipFocus = false;
    }
    DrawTextPro(GetFontDefault(), "IP:", {1026, 740}, {580, 380}, 0.0f, 20.0f, 4.0f, color_names);
    if (GuiTextBox({(GetScreenWidth() / 2.0f) - 110.0f, GetScreenHeight() / 2.0f, 220.0f, 60.0f}, ip, 16, ipFocus))
    {
        ipFocus = true;
        nameFocus = false;
    }
    buttons(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 90.0f, 120.0f, 50.0f, "JUGAR");
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 90.0f, 120.0f, 50.0f}, "JUGAR"))
    {
        if (connectToServer(client, ip, name, 12345, senderThread, receiverThread))
        {
            screen->scene = GAME;
        }
    }
    buttons(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 160.0f, 120.0f, 50.0f, "SALIR");
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 160.0f, 120.0f, 50.0f}, "SALIR"))
    {
        screen->scene = EXIT;
    }
}

void drawStart(Screen *screen, shared_ptr<Client> &client, thread *senderThread, thread *receiverThread)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(screen->background, 0, 0, WHITE);
    DrawTexture(screen->title, (GetScreenWidth() / 2.0f) - (screen->title.width / 2.0f), 50, WHITE);

    Color color1 = {102, 149, 89, 200};
    DrawRectangleRounded({
                             (GetScreenWidth() / 2.0f) - 175.0f,
                             (GetScreenHeight() / 2.0f) - 150.0f,
                             350.0f,
                             400.0f,
                         },
                         0.5f, 0.5, color1);

    startGUI(screen, client, senderThread, receiverThread);
    EndDrawing();
}
