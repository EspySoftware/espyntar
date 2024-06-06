#pragma once
#include "./Inicio.h"

void drawGame(Screen *screen)
{
    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexture(screen->background, 0, 0, WHITE);

    EndDrawing();
}