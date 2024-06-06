#include "../headers/Canvas.h"
#include <queue>

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

bool Canvas::IsWithinBounds(int x, int y)
{
    return x >= 0 && x < target.texture.width && y >= 0 && y < target.texture.height;
}

Color Canvas::GetColorAt(int x, int y)
{
    Image image = LoadImageFromTexture(target.texture);
    Color color = GetImageColor(image, x, y);
    UnloadImage(image);
    return color;
}

void Canvas::BucketFill(Vector2 position, Color newColor)
{
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);

    if (!IsWithinBounds(x, y))
        return;

    Color targetColor = GetColorAt(x, y);

    if (targetColor.r == newColor.r && targetColor.g == newColor.g && targetColor.b == newColor.b && targetColor.a == newColor.a)
        return;

    std::queue<Vector2> nodes;
    nodes.push(position);

    while (!nodes.empty())
    {
        Vector2 node = nodes.front();
        nodes.pop();

        int nx = static_cast<int>(node.x);
        int ny = static_cast<int>(node.y);

        if (!IsWithinBounds(nx, ny))
            continue;

        Color currentColor = GetColorAt(nx, ny);

        if (currentColor.r == targetColor.r && currentColor.g == targetColor.g && currentColor.b == targetColor.b && currentColor.a == targetColor.a)
        {
            BeginTextureMode(target);
            DrawPixel(nx, ny, newColor);
            EndTextureMode();

            nodes.push(Vector2{node.x + 1, node.y});
            nodes.push(Vector2{node.x - 1, node.y});
            nodes.push(Vector2{node.x, node.y + 1});
            nodes.push(Vector2{node.x, node.y - 1});
        }
    }
}