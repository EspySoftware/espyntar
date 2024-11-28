#pragma once

#include "ColorPalette.h"
#include "Canvas.h"
#include <sstream>
#include <iostream>
#include <memory>

using std::shared_ptr;
using std::string;
using std::stringstream;

class Client;

class Painter
{
private:
    ColorPalette &palette;        // Referencia a la paleta de colores
    Canvas &canvas;               // Referencia al lienzo
    float brushSize;              // Tamaño del pincel
    Vector2 lastPosition;         // Última posición del pincel (local)
    Vector2 serverLastPosition;   // Última posición del pincel (servidor)
    int currentColor;             // Índice del color actual
    bool canPaint;                // Estado para permitir o no pintar

    // Métodos auxiliares
    Vector2 AdjustPosition(Vector2 position);                           // Ajusta la posición en relación al lienzo
    void InterpolateAndDraw(Vector2 start, Vector2 end, float size, Color color); // Interpola y dibuja entre dos posiciones

public:
    Painter(ColorPalette &palette, Canvas &canvas);

    // Métodos principales
    void Paint(Vector2 position);                                       // Pintar en el lienzo local
    void Paint(Vector2 position, int color, float size);                // Pintar desde otros clientes
    void Paint(Vector2 position, shared_ptr<Client> client);            // Pintar desde cliente local y enviar al servidor
    void Erase(Vector2 position);                                       // Borrar en el lienzo
    void Fill(Vector2 position);                                        // Rellenar un área en el lienzo

    // Métodos auxiliares
    void ResetLastPosition() { lastPosition = {-1.0f, -1.0f}; }
    void ResetServerLastPosition() { serverLastPosition = {-1.0f, -1.0f}; }
    void SetBrushSize(float delta);                                     // Ajustar el tamaño del pincel
    void SetColor(int colorIndex) { currentColor = colorIndex; }        // Cambiar el color actual
    float GetBrushSize() { return brushSize; }                          // Obtener el tamaño del pincel
    Color GetColor() { return palette.GetColor(currentColor); }         // Obtener el color actual
    void SetCanPaint(bool canPaint) { this->canPaint = canPaint; }      // Cambiar estado de pintar
};
