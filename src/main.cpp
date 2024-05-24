#include "raylib.h"
#include <vector>

#define MAX_COLORS_COUNT 23

class ColorPalette
{
public:
    ColorPalette()
    {
        colors = {RAYWHITE, YELLOW, GOLD, ORANGE, PINK, RED, MAROON, GREEN, LIME, DARKGREEN,
                  SKYBLUE, BLUE, DARKBLUE, PURPLE, VIOLET, DARKPURPLE, BEIGE, BROWN, DARKBROWN,
                  LIGHTGRAY, GRAY, DARKGRAY, BLACK};
    }

    Color getColor(int index)
    {
        return colors[index];
    }

private:
    std::vector<Color> colors;
};

class Canvas
{
public:
    Canvas(int width, int height, ColorPalette &palette)
    {
        target = LoadRenderTexture(width, height);
        BeginTextureMode(target);
        ClearBackground(palette.getColor(0)); // Use the palette to get the color
        EndTextureMode();
    }

    ~Canvas()
    {
        UnloadRenderTexture(target);
    }

    void DrawCircleV(Vector2 position, float radius, Color color)
    {
        BeginTextureMode(target);
        ::DrawCircleV(position, radius, color);
        EndTextureMode();
    }

    RenderTexture2D getTarget()
    {
        return target;
    }

    void DrawPalette(ColorPalette &palette)
    {
        int colorsCount = MAX_COLORS_COUNT;
        int columns = colorsCount;

        int colorIndex = 0;
        float recWidth = (float)GetScreenWidth() / columns;
        float recHeight = 30.0f;

        for (int i = 0; i < columns; i++)
        {
            Rectangle rec = {recWidth * i, 0, recWidth, recHeight};

            // Draw pallete background
            DrawRectangleRec(rec, palette.getColor(colorIndex));
            DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, BLACK);

            colorIndex++;
            if (colorIndex >= colorsCount)
                break;
        }
    }

    int CheckPaletteClick(ColorPalette &palette)
    {
        int colorsCount = MAX_COLORS_COUNT;
        int columns = colorsCount;

        float recWidth = (float)GetScreenWidth() / columns;
        float recHeight = 30.0f;

        Vector2 mousePos = GetMousePosition();

        for (int i = 0; i < columns; i++)
        {
            Rectangle rec = {recWidth * i, 0, recWidth, recHeight};

            if (CheckCollisionPointRec(mousePos, rec))
            {
                return i; // Return the index of the clicked color
            }
        }

        return -1; // Return -1 if no color is clicked
    }

private:
    RenderTexture2D target;
};

class Painter
{
public:
    Painter(ColorPalette &palette, Canvas &canvas) : palette(palette), canvas(canvas)
    {
        brushSize = 20.0f;
        currentColor = 0;
        lastPosition = {-1.0f, -1.0f};
    }

    void Paint(Vector2 position)
    {
        if (lastPosition.x >= 0 && lastPosition.y >= 0)
        {
            // Interpolate between lastPosition and position
            for (float t = 0.0f; t < 1.0f; t += 0.05f)
            {
                Vector2 interpolatedPosition;
                interpolatedPosition.x = lastPosition.x * (1 - t) + position.x * t;
                interpolatedPosition.y = lastPosition.y * (1 - t) + position.y * t;
                canvas.DrawCircleV(interpolatedPosition, brushSize, palette.getColor(currentColor));
            }
        }
        else
        {
            canvas.DrawCircleV(position, brushSize, palette.getColor(currentColor));
        }

        lastPosition = position;
    }

    void ChangeColor(int colorIndex)
    {
        currentColor = colorIndex;
    }

    void ResetLastPosition()
    {
        lastPosition = {-1.0f, -1.0f};
    }

    void DrawPalette(ColorPalette &palette)
    {
        canvas.DrawPalette(palette);
    }

    void ChangeBrushSize(float delta)
    {
        delta *= 2.0f;

        brushSize += delta;
        if (brushSize < 1.0f)
            brushSize = 1.0f;

        if (brushSize > 100.0f)
            brushSize = 100.0f;
    }

    float GetBrushSize()
    {
        return brushSize;
    }

private:
    ColorPalette &palette;
    Canvas &canvas;
    float brushSize;
    Vector2 lastPosition;
    int currentColor = 2;
};

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Espyntar");

    ColorPalette palette;
    Canvas canvas(screenWidth, screenHeight, palette);
    Painter painter(palette, canvas);

    SetTargetFPS(144);

    while (!WindowShouldClose())
    {
        Vector2 position = GetMousePosition();

        float mouseWheelMove = GetMouseWheelMove();
        painter.ChangeBrushSize(mouseWheelMove);

        // Paint if the left mouse button is pressed
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            // Check if a color in the palette is clicked
            int colorIndex = canvas.CheckPaletteClick(palette);
            if (colorIndex >= 0)
            {
                painter.ChangeColor(colorIndex);
            }
            else
            {
                painter.Paint(position);
            }
        }
        else
        {
            painter.ResetLastPosition();
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the canvas
        Rectangle rec = {0.0f, 0.0f, (float)canvas.getTarget().texture.width, (float)-canvas.getTarget().texture.height};
        Vector2 vec = {0.0f, 0.0f};
        DrawTextureRec(canvas.getTarget().texture, rec, vec, WHITE);

        // Draw the brush
        DrawCircleLines(GetMouseX(), GetMouseY(), painter.GetBrushSize(), BLACK);

        // Palette
        painter.DrawPalette(palette);
        DrawFPS(GetScreenWidth() - 95, 10);
        EndDrawing();
    }
    CloseWindow();

    return 0;
}