#pragma once
#include "ColorPalette.h"
#include "Canvas.h"

class Painter
{
private:
    ColorPalette &palette;
    Canvas &canvas;
    float brushSize;
    Vector2 lastPosition;
    int currentColor = 2;

public:
    Painter(ColorPalette &palette, Canvas &canvas);
    void Paint(Vector2 position);

    void ResetLastPosition() { lastPosition = {-1.0f, -1.0f}; }
    void SetBrushSize(float delta);
    void SetColor(int colorIndex) { currentColor = colorIndex; }
    float GetBrushSize() { return brushSize; }
};