#pragma once
#include "../headers/Inicio.h"

class Configuration
{
public:
    bool musicOn;
    bool effects;
    Texture2D background;
    Texture2D title;

    void drawConfig(Screen *scene);
    Configuration();
    ~Configuration();
};

Configuration::Configuration()
{
    this->musicOn = true;
    this->effects = true;
    this->background = LoadTexture("../assets/background.png");
    this->title = LoadTexture("../assets/espy.png");
}

Configuration::~Configuration()
{
    UnloadTexture(this->background);
    UnloadTexture(this->title);
}

inline void Configuration::drawConfig(Screen *scene)
{
    BeginDrawing();


    DrawTexture(this->background, 0, 0, WHITE);
    DrawTexture(this->title, (GetScreenWidth() - this->title.width) / 2, 50, WHITE);

    buttons(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 90.0f, 160.0f, 50.0f, "MUSICA");
    if (this->musicOn) // musica activada
    {
        if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 90.0f, 160.0f, 50.0f}, "MUSICA on"))
        {
            this->musicOn = false; // desactivar musica
        }
    }
    else // musica desactivada
    {
        if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 90.0f, 160.0f, 50.0f}, "MUSICA off"))
        {
            this->musicOn = true; // activar musica
        }
    }

    buttons(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 20.0f, 160.0f, 50.0f, "EFECTOS");
    if (this->effects) // efectos activados
    {
        if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 20.0f, 160.0f, 50.0f}, "EFECTOS on"))
        {
            this->effects = false; // desactivar efectos
        }
    }
    else // efectos desactivados
    {
        if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 20.0f, 160.0f, 50.0f}, "EFECTOS off"))
        {
            this->effects = true; // activar efectos
        }
    }

    buttons(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 160.0f, 160.0f, 50.0f, "VOLVER");
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 160.0f, 160.0f, 50.0f}, "VOLVER"))
    {
        scene->scene = GAME;
    }

    buttons(GetScreenWidth() / 2.0f - 60, GetScreenHeight() / 2.0f + 190.0f, 160.0f, 50.0f, "SALIR");
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() / 2.0f + 190.0f, 160.0f, 50.0f}, "SALIR"))
    {
        scene->scene = EXIT;
    }

    EndDrawing();
}
