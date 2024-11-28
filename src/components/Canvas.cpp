#include "../headers/Canvas.h"
#include <queue>

Canvas::Canvas(int width, int height)
    : width(width), height(height)
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
    const int columns = MAX_COLORS_COUNT;
    const float recWidth = static_cast<float>(width) / columns;
    const float recHeight = 30.0f;
    const float startX = 230.0f;
    const float startY = GetScreenHeight() / 2 + 320.0f;

    for (int i = 0; i < columns && i < MAX_COLORS_COUNT; ++i)
    {
        Rectangle rec = { startX + recWidth * i, startY, recWidth, recHeight };
        Color color = palette.GetColor(i);
        DrawRectangleRec(rec, color);
        DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, BLACK);
    }
}

int Canvas::CheckPaletteClick(ColorPalette &palette)
{
    const int columns = MAX_COLORS_COUNT;
    const float recWidth = static_cast<float>(width) / columns;
    const float recHeight = 30.0f;
    const float startX = 230.0f;
    const float startY = GetScreenHeight() / 2 + 320.0f;

    Vector2 mousePos = GetMousePosition();
    for (int i = 0; i < columns && i < MAX_COLORS_COUNT; ++i)
    {
        Rectangle rec = { startX + recWidth * i, startY, recWidth, recHeight };
        if (CheckCollisionPointRec(mousePos, rec))
        {
            return i;
        }
    }

    return -1; // No color clicked
}

bool Canvas::IsWithinBounds(int x, int y)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

Color Canvas::GetColorAt(int x, int y)
{
    Image image = LoadImageFromTexture(target.texture);
    Color color = GetImageColor(image, x, y);
    UnloadImage(image);
    return color;
}

void Canvas::DrawPixel(int x, int y, Color color)
{
    BeginTextureMode(target);
    DrawPixelV(Vector2{(float)x, (float)y}, color);
    EndTextureMode();
}

void Canvas::BucketFill(Vector2 position, Color newColor)
{
    int x = static_cast<int>(position.x);
    int y = static_cast<int>(position.y);

    if (!IsWithinBounds(x, y))
        return;

    Color targetColor = GetColorAt(x, y);

    // Skip if the color is already the same
    if (targetColor.r == newColor.r && targetColor.g == newColor.g && targetColor.b == newColor.b && targetColor.a == newColor.a)
        return;

    std::queue<Vector2> nodes;
    nodes.push(position);

    BeginTextureMode(target);

    // Perform flood fill
    while (!nodes.empty())
    {
        Vector2 node = nodes.front();
        nodes.pop();

        int nx = static_cast<int>(node.x);
        int ny = static_cast<int>(node.y);

        if (!IsWithinBounds(nx, ny))
            continue;

        Color currentColor = GetColorAt(nx, ny);
        if (currentColor.r == targetColor.r && currentColor.g == targetColor.g &&
            currentColor.b == targetColor.b && currentColor.a == targetColor.a)
        {
            DrawPixel(nx, ny, newColor);

            // Add adjacent pixels to the queue
            nodes.push(Vector2{node.x + 1, node.y});
            nodes.push(Vector2{node.x - 1, node.y});
            nodes.push(Vector2{node.x, node.y + 1});
            nodes.push(Vector2{node.x, node.y - 1});
        }
    }

    EndTextureMode();
}

void Canvas::Clear()
{
    BeginTextureMode(target);
    ClearBackground(WHITE);
    EndTextureMode();
}
