#include "../headers/Painter.h"
#include "../headers/Client.h"

Painter::Painter(ColorPalette &palette, Canvas &canvas) : palette(palette), canvas(canvas)
{
    brushSize = 10.0f;
    currentColor = 22;
    lastPosition = {-1.0f, -1.0f};
    SetColor(22);
}

// Paint on the local canvas
void Painter::Paint(Vector2 position)
{
    position.x -= GetScreenWidth() / 2.0f - canvas.GetTarget().texture.width / 2.0f;
    position.y -= GetScreenHeight() / 2.0f - canvas.GetTarget().texture.height / 2.0f + 70.0f;

    if (lastPosition.x >= 0 && lastPosition.y >= 0)
    {
        // Interpolate between lastPosition and position
        for (float t = 0.0f; t < 1.0f; t += 0.05f)
        {
            Vector2 interpolatedPosition;
            interpolatedPosition.x = lastPosition.x * (1 - t) + position.x * t;
            interpolatedPosition.y = lastPosition.y * (1 - t) + position.y * t;
            canvas.Draw(interpolatedPosition, brushSize, palette.GetColor(currentColor));
        }
    }
    else
    {
        canvas.Draw(position, brushSize, palette.GetColor(currentColor));
    }

    lastPosition = position;
}

// Paint from other clients
void Painter::Paint(Vector2 position, int color, float size)
{
    position.x -= GetScreenWidth() / 2.0f - canvas.GetTarget().texture.width / 2.0f;
    position.y -= GetScreenHeight() / 2.0f - canvas.GetTarget().texture.height / 2.0f + 70.0f;

    if (lastPosition.x >= 0 && lastPosition.y >= 0)
    {
        // Interpolate between lastPosition and position
        for (float t = 0.0f; t < 1.0f; t += 0.05f)
        {
            Vector2 interpolatedPosition;
            interpolatedPosition.x = lastPosition.x * (1 - t) + position.x * t;
            interpolatedPosition.y = lastPosition.y * (1 - t) + position.y * t;
            canvas.Draw(interpolatedPosition, size, palette.GetColor(color));
        }
    }
    else
    {
        canvas.Draw(position, size, palette.GetColor(color));
    }

    lastPosition = position;
}

// Paint from the local client and send the message to the server
void Painter::Paint(Vector2 position, shared_ptr<Client> client)
{
    Paint(position);

    // Create a message containing the paint action data
    stringstream ss;
    ss << "PAINT:" << position.x << "," << position.y << "," << currentColor << "," << brushSize;

    // Send the message to the server
    client->Send(ss.str());
}

void Painter::Erase(Vector2 position)
{
    position.x -= GetScreenWidth() / 2.0f - canvas.GetTarget().texture.width / 2.0f;
    position.y -= GetScreenHeight() / 2.0f - canvas.GetTarget().texture.height / 2.0f + 70.0f;

    if (lastPosition.x >= 0 && lastPosition.y >= 0)
    {
        // Interpolate between lastPosition and position
        for (float t = 0.0f; t < 1.0f; t += 0.05f)
        {
            Vector2 interpolatedPosition;
            interpolatedPosition.x = lastPosition.x * (1 - t) + position.x * t;
            interpolatedPosition.y = lastPosition.y * (1 - t) + position.y * t;
            canvas.Draw(interpolatedPosition, brushSize, palette.GetColor(currentColor));
        }
    }
    else
    {
        canvas.Draw(position, brushSize, palette.GetColor(0));
    }
    lastPosition = position;
}

void Painter::SetBrushSize(float delta)
{
    delta *= 2.0f;

    brushSize += delta;
    if (brushSize < 1.0f)
        brushSize = 1.0f;

    if (brushSize > 100.0f)
        brushSize = 100.0f;
}

void Painter::Fill(Vector2 position)
{
    canvas.BucketFill(position, GetColor());
}