#include "../headers/Painter.h"
#include "../headers/Client.h"

Painter::Painter(ColorPalette &palette, Canvas &canvas) 
    : palette(palette), canvas(canvas), brushSize(10.0f), currentColor(22), lastPosition({-1.0f, -1.0f}), serverLastPosition({-1.0f, -1.0f})
{
    SetColor(0);
}

// Métodos auxiliares
Vector2 Painter::AdjustPosition(Vector2 position)
{
    position.x -= GetScreenWidth() / 2.0f - canvas.GetTarget().texture.width / 2.0f;
    position.y -= GetScreenHeight() / 2.0f - canvas.GetTarget().texture.height / 2.0f + 70.0f;
    return position;
}

void Painter::InterpolateAndDraw(Vector2 start, Vector2 end, float size, Color color)
{
    for (float t = 0.0f; t < 1.0f; t += 0.05f)
    {
        Vector2 interpolatedPosition;
        interpolatedPosition.x = start.x * (1 - t) + end.x * t;
        interpolatedPosition.y = start.y * (1 - t) + end.y * t;
        canvas.Draw(interpolatedPosition, size, color);
    }
}

// Pintar en el lienzo local
void Painter::Paint(Vector2 position)
{
    if (!canPaint)
        return;

    position = AdjustPosition(position);

    if (lastPosition.x >= 0 && lastPosition.y >= 0)
    {
        InterpolateAndDraw(lastPosition, position, brushSize, palette.GetColor(currentColor));
    }
    else
    {
        canvas.Draw(position, brushSize, palette.GetColor(currentColor));
    }

    lastPosition = position;
}

// Pintar desde otros clientes
void Painter::Paint(Vector2 position, int color, float size)
{
    position = AdjustPosition(position);

    if (serverLastPosition.x >= 0 && serverLastPosition.y >= 0)
    {
        InterpolateAndDraw(serverLastPosition, position, size, palette.GetColor(color));
    }
    else
    {
        canvas.Draw(position, size, palette.GetColor(color));
    }

    serverLastPosition = position;
}

// Pintar desde el cliente local y enviar al servidor
void Painter::Paint(Vector2 position, std::shared_ptr<Client> client)
{
    if (!canPaint)
        return;

    Paint(position);

    std::stringstream ss;
    ss << "PAINT:" << position.x << "," << position.y << "," << currentColor << "," << brushSize;

    client->Send(ss.str());
}

// Borrar contenido del lienzo
void Painter::Erase(Vector2 position)
{
    if (!canPaint)
        return;

    position = AdjustPosition(position);

    if (lastPosition.x >= 0 && lastPosition.y >= 0)
    {
        InterpolateAndDraw(lastPosition, position, brushSize, palette.GetColor(0)); // Color 0: borrar
    }
    else
    {
        canvas.Draw(position, brushSize, palette.GetColor(0));
    }

    lastPosition = position;
}

// Ajustar el tamaño del pincel
void Painter::SetBrushSize(float delta)
{
    brushSize += delta * 2.0f;

    if (brushSize < 1.0f)
        brushSize = 1.0f;
    else if (brushSize > 100.0f)
        brushSize = 100.0f;
}

// Rellenar un área en el lienzo
void Painter::Fill(Vector2 position)
{
    canvas.BucketFill(position, GetColor());
}
