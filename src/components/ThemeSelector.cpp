#include "../headers/ThemeSelector.h"
#include "../headers/raygui.h"
#include <iostream>

using std::cout;
using std::endl;

ThemeSelector::ThemeSelector()
{
    // Temáticas por defecto
    availableThemes = {
        "General",
        "Animales",
        "Deportes",
        "Comida",
        "Naturaleza",
        "Transporte",
        "Profesiones",
        "Hogar",
        "Tecnología",
        "Música",
        "Ropa"};

    selectedThemeIndex = 0;
    isVisible = false;

    // Configurar posición por defecto (centro de la pantalla)
    bounds = {300, 200, 400, 300};
}

void ThemeSelector::SetThemes(const vector<string> &themes)
{
    availableThemes = themes;
    if (selectedThemeIndex >= availableThemes.size())
    {
        selectedThemeIndex = 0;
    }
}

void ThemeSelector::Draw()
{
    if (!isVisible)
        return;

    // Dibujar fondo semi-transparente
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 128});

    // Dibujar panel principal
    DrawRectangleRec(bounds, {102, 149, 89, 255});
    DrawRectangleLinesEx(bounds, 3, BLACK);

    // Título
    string title = "Selecciona una tematica";
    int titleWidth = MeasureText(title.c_str(), 24);
    DrawText(title.c_str(),
             bounds.x + (bounds.width - titleWidth) / 2,
             bounds.y + 20,
             24,
             BLACK);

    // Botones de temáticas
    float buttonHeight = 35;
    float buttonSpacing = 5;
    float startY = bounds.y + 60;
    int buttonsPerRow = 2;
    float buttonWidth = (bounds.width - 40) / buttonsPerRow - buttonSpacing;

    for (int i = 0; i < availableThemes.size(); i++)
    {
        int row = i / buttonsPerRow;
        int col = i % buttonsPerRow;

        float buttonX = bounds.x + 20 + col * (buttonWidth + buttonSpacing);
        float buttonY = startY + row * (buttonHeight + buttonSpacing);

        Rectangle buttonRect = {buttonX, buttonY, buttonWidth, buttonHeight};

        // Cambiar color si está seleccionado
        Color buttonColor = (i == selectedThemeIndex) ? DARKGREEN : LIGHTGRAY;

        if (GuiButton(buttonRect, availableThemes[i].c_str()))
        {
            selectedThemeIndex = i;
            cout << "Selected theme: " << availableThemes[i] << endl;
        }
    }

    // Botones de control
    float controlButtonWidth = 120;
    float controlButtonHeight = 40;
    float controlY = bounds.y + bounds.height - 60;

    // Botón Confirmar
    Rectangle confirmButton = {
        bounds.x + bounds.width - controlButtonWidth - 20,
        controlY,
        controlButtonWidth,
        controlButtonHeight};

    if (GuiButton(confirmButton, "Confirmar"))
    {
        Hide();
        cout << "Theme confirmed: " << GetSelectedTheme() << endl;
    }

    // Botón Cancelar
    Rectangle cancelButton = {
        bounds.x + 20,
        controlY,
        controlButtonWidth,
        controlButtonHeight};

    if (GuiButton(cancelButton, "Cancelar"))
    {
        Hide();
        cout << "Theme selection cancelled" << endl;
    }

    // Información adicional
    string info = "Las palabras seran generadas por IA";
    int infoWidth = MeasureText(info.c_str(), 16);
    DrawText(info.c_str(),
             bounds.x + (bounds.width - infoWidth) / 2,
             controlY - 25,
             16,
             DARKGRAY);
}

void ThemeSelector::HandleInput()
{
    if (!isVisible)
        return;

    // Manejar teclas de navegación
    if (IsKeyPressed(KEY_UP))
    {
        selectedThemeIndex = (selectedThemeIndex - 2 + availableThemes.size()) % availableThemes.size();
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        selectedThemeIndex = (selectedThemeIndex + 2) % availableThemes.size();
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        selectedThemeIndex = (selectedThemeIndex - 1 + availableThemes.size()) % availableThemes.size();
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        selectedThemeIndex = (selectedThemeIndex + 1) % availableThemes.size();
    }

    // Confirmar con Enter
    if (IsKeyPressed(KEY_ENTER))
    {
        Hide();
        cout << "Theme confirmed with Enter: " << GetSelectedTheme() << endl;
    }

    // Cancelar con Escape
    if (IsKeyPressed(KEY_ESCAPE))
    {
        Hide();
        cout << "Theme selection cancelled with Escape" << endl;
    }
}

string ThemeSelector::GetSelectedTheme() const
{
    if (selectedThemeIndex >= 0 && selectedThemeIndex < availableThemes.size())
    {
        return availableThemes[selectedThemeIndex];
    }
    return "General";
}

void ThemeSelector::SetSelectedIndex(int index)
{
    if (index >= 0 && index < availableThemes.size())
    {
        selectedThemeIndex = index;
    }
}