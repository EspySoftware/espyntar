#pragma once

#include "ColorPalette.h"
#include <queue>
#include <raylib.h>

#define MAX_COLORS_COUNT 23

class Canvas
{
private:
    RenderTexture2D target;           // Render texture for drawing
    int width;                        // Canvas width
    int height;                       // Canvas height

    // Private helper functions
    Color GetColorAt(int x, int y);   // Get color from RenderTexture2D at position (x, y)
    bool IsWithinBounds(int x, int y); // Check if position is within RenderTexture2D bounds
    void DrawPixel(int x, int y, Color color); // Draw a single pixel on the texture

public:
    // Constructor and Destructor
    Canvas(int width, int height);
    ~Canvas();

    // Public drawing functions
    void Draw(Vector2 position, float radius, Color color); // Draw a circle on canvas
    void DrawPalette(ColorPalette &palette);               // Draw color palette on canvas
    int CheckPaletteClick(ColorPalette &palette);          // Check if a palette color was clicked
    void BucketFill(Vector2 position, Color newColor);     // Perform bucket fill on canvas
    void Clear();                                          // Clear the canvas to white

    // Accessor function to get the RenderTexture2D
    RenderTexture2D GetTarget() { return target; }
};
