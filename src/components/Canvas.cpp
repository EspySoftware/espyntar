
#include "../headers/Canvas.h"
#include <queue>

Canvas::Canvas(int width, int height, ColorPalette &palette)
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
    int colorsCount = MAX_COLORS_COUNT;
    int columns = colorsCount;

    int colorIndex = 0;
    float recWidth = (float)width / columns;
    float recHeight = 30.0f;

    for (int i = 0; i < columns; i++)
    {
        Rectangle rec = {(recWidth * i) + 230.0f, GetScreenHeight() / 2 + 320.0f, recWidth, recHeight};

        // Draw palette background
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

    float recWidth = (float)width / columns;
    float recHeight = 30.0f;

    Vector2 mousePos = GetMousePosition();

    for (int i = 0; i < columns; i++)
    {
        Rectangle rec = {(recWidth * i) + 230.0f, GetScreenHeight() / 2 + 320.0f, recWidth, recHeight};

        if (CheckCollisionPointRec(mousePos, rec))
        {
            return i; // Return the index of the clicked color
        }
    }

    return -1; // Return -1 if no color is clicked
}

bool Canvas::IsWithinBounds(int x, int y)
{
    return x >= 0 && x < width && y >= 0 && y < height;
}

Color Canvas::GetColorAt(int x, int y)
{
    Image image = LoadImageFromTexture(target.texture);
    Color color = GetImageColor(image, x, y); // Get the color of the pixel at (x, y)
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

    if (targetColor.r == newColor.r && targetColor.g == newColor.g && targetColor.b == newColor.b && targetColor.a == newColor.a)
        return;

    std::queue<Vector2> nodes;
    nodes.push(position);

    BeginTextureMode(target); // Iniciar modo de dibujo en la textura
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
            DrawPixel(nx, ny, newColor);

            nodes.push(Vector2{node.x + 1, node.y});
            nodes.push(Vector2{node.x - 1, node.y});
            nodes.push(Vector2{node.x, node.y + 1});
            nodes.push(Vector2{node.x, node.y - 1});
        }
    }
    EndTextureMode(); // Finalizar modo de dibujo en la textura
}
