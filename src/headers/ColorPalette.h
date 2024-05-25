#pragma once
#include <raylib.h>
#include <vector>

using std::vector;

class ColorPalette
{
public:
    ColorPalette()
    {
        colors = {WHITE, YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN,
                  SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN,
                  LIGHTGRAY, GRAY, DARKGRAY, BLACK};
    }

    Color getColor(int index) { return colors[index]; }

private:
    vector<Color> colors;
};