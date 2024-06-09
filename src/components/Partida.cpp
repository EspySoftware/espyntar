#include "../headers/Partida.h"

Partida::Partida(Games &game) : game(game)
{
}

void Partida::Ronda(shared_ptr<Client> &client)
{
    if (currentRound <= maxRounds)
    {
        if (!game.GetChosen())
        {
            game.SetChosenWord();
        }
        if (game.GetChosen())
        {
            game.DrawChosenWord(client);
        }
    }

    if (game.GetFinished())
    {
        game.SetDefault();
        currentRound++;
    }
}

void Partida::DrawRounds()
{
    if (currentRound != 3)
    {
        std::string roundText = "Ronda " + std::to_string(currentRound + 1) + " de 3";
        const char *textToDraw = roundText.c_str();
        DrawTextPro(GetFontDefault(), textToDraw, {58, 80}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {
        DrawTextPro(GetFontDefault(), "Partida finalizada", {58, 80}, {0, 0}, 0, 20, 4, BLACK);
    }
}
