#pragma once
#include <string>
#include <vector>
#include <raylib.h>

using std::string;
using std::vector;

/**
 * Interfaz para seleccionar temáticas para el generador de palabras IA
 */
class ThemeSelector
{
private:
    vector<string> availableThemes;
    int selectedThemeIndex;
    bool isVisible;
    Rectangle bounds;

public:
    ThemeSelector();

    // Configurar temáticas disponibles
    void SetThemes(const vector<string> &themes);

    // Mostrar/ocultar selector
    void Show() { isVisible = true; }
    void Hide() { isVisible = false; }
    bool IsVisible() const { return isVisible; }

    // Dibujar interfaz
    void Draw();

    // Manejar entrada
    void HandleInput();

    // Obtener temática seleccionada
    string GetSelectedTheme() const;
    int GetSelectedIndex() const { return selectedThemeIndex; }
    void SetSelectedIndex(int index);

    // Configurar posición y tamaño
    void SetBounds(Rectangle newBounds) { bounds = newBounds; }
    Rectangle GetBounds() const { return bounds; }
};