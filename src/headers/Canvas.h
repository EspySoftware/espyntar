#pragma once
#include <raylib.h>
#include "ColorPalette.h"

#define MAX_COLORS_COUNT 23

class Canvas
{
private:
    RenderTexture2D target;

public:
    Canvas(int width, int height, ColorPalette &palette);
    ~Canvas();

    void Draw(Vector2 position, float radius, Color color);
    void DrawPalette(ColorPalette &palette);
    int CheckPaletteClick(ColorPalette &palette);

    RenderTexture2D GetTarget() { return target; }
};