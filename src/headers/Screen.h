#pragma once

typedef enum // estructura logica del juego
{
    START,
    GAME,
    WINNER,
    EXIT
} GameScene;

class Screen // clase para manejar las pantallas del juego
{
public:
    GameScene scene;
    Texture2D background;
    Texture2D title;

    Screen()
    {
        this->scene = START;
        this->background = LoadTexture("assets/background.png");
        this->title = LoadTexture("assets/espy.png");
    }

    ~Screen()
    {
        UnloadTexture(this->background);
        UnloadTexture(this->title);
    }
};
