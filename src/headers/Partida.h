#include "./Games.h"

class Partida
{
private:
    int currentRound = 0;
    int maxRounds = 3;
    int players;
    bool started = false;

    Games &game;

public:

    Partida(Games &game );
    void Ronda(shared_ptr<Client> &client);
    Games &GetGame() { return game; }
    void DrawRounds();
};

