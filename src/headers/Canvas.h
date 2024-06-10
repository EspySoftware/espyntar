#pragma once

#include "ColorPalette.h"
#include <queue>
#include <raylib.h>
#include <iostream>

using std::cout;
using std::endl;

#define MAX_COLORS_COUNT 23

class Canvas
{
private:
    RenderTexture2D target;
    Color GetColorAt(int x, int y);    // Get color from RenderTexture2D at position
    bool IsWithinBounds(int x, int y); // Check if position is within RenderTexture2D bounds
    void DrawPixel(int x, int y, Color color);
    int width;
    int height;

public:
    Canvas(int width, int height);
    ~Canvas();

    void Draw(Vector2 position, float radius, Color color);
    void DrawPalette(ColorPalette &palette);
    int CheckPaletteClick(ColorPalette &palette);
    void BucketFill(Vector2 position, Color newColor);
    void Clear();

    RenderTexture2D GetTarget() { return target; }
};