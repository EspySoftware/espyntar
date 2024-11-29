#pragma once

#define RAYGUI_IMPLEMENTATION
#include "./raygui.h"

#include <iostream>
#include <thread>
#include "./ColorPalette.h"
#include "./Canvas.h"
#include "./Painter.h"
#include "./Client.h"
#include "./Screen.h"

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::thread;

// Constantes de pantalla
constexpr int screenWidth = 1160;
constexpr int screenHeight = 760;

// Función para conectar al servidor
bool connectToServer(std::shared_ptr<Client>& client, const string& ip, const string& name, int port, thread* senderThread, thread* receiverThread)
{
    cout << "Trying to connect to server " << ip << " with name " << name << endl;
    client = std::make_shared<Client>(ip, port, name);

    if (client->clientSocket == INVALID_SOCKET || client->clientSocket == SOCKET_ERROR)
    {
        cout << "Failed to create socket. Error code: " << WSAGetLastError() << endl;
        return false;
    }

    *senderThread = thread([client] { client->Send(); });
    *receiverThread = thread([client] { client->Receive(); });

    return true;
}

// Función para dibujar los botones con estilo
void drawButton(float x, float y, float width, float height, const char* text, Color color)
{
    Rectangle buttonRect = {x, y, width, height};
    Rectangle buttonRect2 = {x - 2.0f, y - 2.0f, width + 4.0f, height + 4.0f};

    DrawRectangleRounded(buttonRect2, 0.3f, 6, color);
}

// Configuración de los estilos de la GUI
void setupGUIStyles()
{
    Color color_base = {44, 74, 36, 200};
    Color color_names = {252, 229, 113, 255};
    Color color_button = {249, 217, 53, 255};

    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    GuiSetStyle(DEFAULT, TEXT_ALIGN_CENTER, TEXT_ALIGN_CENTER);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 3);

    // Estilo de los cuadros de texto
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, ColorToInt(WHITE));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, ColorToInt(WHITE));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, ColorToInt(color_base));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_DISABLED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, ColorToInt(BLACK));
    GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, ColorToInt(color_base));

    // Estilo de los botones
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
}

// Función para dibujar la interfaz gráfica
void startGUI(Screen* screen, std::shared_ptr<Client>& client, thread* senderThread, thread* receiverThread)
{
    static char name[16] = "";
    static char ip[20] = "192.168.0.24";
    static bool nameFocus = true;
    static bool ipFocus = false;

    // Ciclado de enfoque entre los cuadros de texto
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_TAB))
    {
        nameFocus = !nameFocus;
        ipFocus = !ipFocus;
    }

    // Dibujar cuadros de texto y botones
    setupGUIStyles();

    // Dibujar cuadro de texto para el nombre
    DrawTextPro(GetFontDefault(), "Nombre:", {1026, 650}, {580, 380}, 0.0f, 20.0f, 4.0f, {252, 229, 113, 255});
    if (GuiTextBox({(GetScreenWidth() / 2.0f) - 110.0f, GetScreenHeight() / 2.0f - 90.0f, 220.0f, 60.0f}, name, 9, nameFocus))
    {
        nameFocus = true;
        ipFocus = false;
    }

    // Dibujar cuadro de texto para la IP
    DrawTextPro(GetFontDefault(), "IP:", {1026, 740}, {580, 380}, 0.0f, 20.0f, 4.0f, {252, 229, 113, 255});
    if (GuiTextBox({(GetScreenWidth() / 2.0f) - 110.0f, GetScreenHeight() / 2.0f, 220.0f, 60.0f}, ip, 16, ipFocus))
    {
        ipFocus = true;
        nameFocus = false;
    }

    // Dibujar botones
    drawButton(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 90.0f, 120.0f, 50.0f, "JUGAR", {215, 182, 15, 255});
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 90.0f, 120.0f, 50.0f}, "JUGAR"))
    {
        if (connectToServer(client, ip, name, 12345, senderThread, receiverThread))
        {
            screen->scene = GAME;
        }
    }

    drawButton(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 160.0f, 120.0f, 50.0f, "SALIR", {215, 182, 15, 255});
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 160.0f, 120.0f, 50.0f}, "SALIR"))
    {
        screen->scene = EXIT;
    }
}

// Función para dibujar la pantalla inicial
void drawStart(Screen* screen, std::shared_ptr<Client>& client, thread* senderThread, thread* receiverThread, Music* musicGame)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(screen->background, 0, 0, WHITE);
    DrawTexture(screen->title, (GetScreenWidth() / 2.0f) - (screen->title.width / 2.0f), 50, WHITE);
    UpdateMusicStream(*musicGame);

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
