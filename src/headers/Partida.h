#include "./Games.h"

class Partida {
private:
    int currentRound;
    int maxRounds = 3;
    int players;
    int time = 0;
    bool started = false;
    Games &game;
public:
    Partida(Games &game);
    void Ronda(shared_ptr<Client> &client);
};