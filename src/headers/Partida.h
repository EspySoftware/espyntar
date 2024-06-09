#include "./Games.h"
#include "./Screen.h"

class Partida
{
private:
    int currentRound = 0;
    int maxRounds = 2;
    int players;
    bool started = false;
    Games &game;

public:
    Screen scene;
    bool musicOn;
    bool effects;
    Texture2D background;
    Texture2D title;
    void DrawConfig();
    Partida(Games &game);
    void Ronda(shared_ptr<Client> &client);
    void DrawRounds();
    Games &GetGame() { return game; }
};
