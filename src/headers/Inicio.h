#pragma once
#define RAYGUI_IMPLEMENTATION
#include <iostream>
#include "./raygui.h"
#include "./ColorPalette.h"
#include "./Canvas.h"
#include "./Painter.h"
#include "./Game.h"

using std::cin;
using std::cout;
using std::endl;

#define screenWidth 800
#define screenHeight 450

typedef enum // estructura logica del juego 
{
    START,
    GAME,
    EXIT
} GameScene;

void buttons(GameScene *scene)
{
    float x;
    float y;
    float width;
    float height;
    Rectangle dimension = {GetScreenWidth() / 2.8f, GetScreenHeight() / 2.0f, 200.0f, 50.0f};
    if (GuiButton(dimension, "PLAY"))
    {
        *scene = GAME;
    }
}

void drawStart(GameScene *scene) // agregar fondo del juego, pedir nombre e IP del servidor 
{
    BeginDrawing();
    ClearBackground(BLACK);
    buttons(scene);
    EndDrawing();
}
