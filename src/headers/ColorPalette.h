#pragma once
#include <raylib.h>
#include <vector>

using std::vector;

class ColorPalette
{
public:
    ColorPalette(vector<Color> colors = {WHITE, YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN, SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN, LIGHTGRAY, GRAY, DARKGRAY, BLACK})
    {
        this->colors = colors;
    };

    Color GetColor(int index) { return colors[index]; }

private:
    vector<Color> colors;
};