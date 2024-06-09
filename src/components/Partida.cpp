#include "../headers/Partida.h"

Partida::Partida(Games &game) : game(game)
{
}

void Partida::Ronda(shared_ptr<Client> &client)
{
    
    // Draw option games
    if (!game.GetChosen())
    {
        game.SetChosenWord();
    }
    if (game.GetChosen())
    {
        game.DrawChosenWord(client);
    }

}
