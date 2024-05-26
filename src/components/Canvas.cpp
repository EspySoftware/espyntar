#include "../headers/Canvas.h"

Canvas::Canvas(int width, int height, ColorPalette &palette)
{
    target = LoadRenderTexture(width, height);
    BeginTextureMode(target);
    ClearBackground(WHITE);
    EndTextureMode();
}

Canvas::~Canvas()
{
    UnloadRenderTexture(target);
}

void Canvas::Draw(Vector2 position, float radius, Color color)
{
    BeginTextureMode(target);
    DrawCircleV(position, radius, color);
    EndTextureMode();
}

void Canvas::DrawPalette(ColorPalette &palette)
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
        DrawRectangleRec(rec, palette.GetColor(colorIndex));
        DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, BLACK);

        colorIndex++;
        if (colorIndex >= colorsCount)
            break;
    }
}

int Canvas::CheckPaletteClick(ColorPalette &palette)
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