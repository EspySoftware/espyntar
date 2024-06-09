#include "./Games.h"

class Partida
{
private:
    int currentRound = 0;
    int maxRounds = 2;
    int players;
    bool started = false;

    Games &game;

public:

    Partida(Games &game );
    void Ronda(shared_ptr<Client> &client);
    void DrawRounds();
    Games &GetGame() { return game; }
};
