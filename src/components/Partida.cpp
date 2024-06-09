#include "../headers/Partida.h"

Partida::Partida(Games &game) : game(game)
{
}

void Partida::Ronda(shared_ptr<Client> &client)
{
    if (currentRound <= maxRounds)
    {
        // game.UpdateChosenWord(client->chosenWord);

        if (!game.GetChosen())
        {
            game.SetChosenWord(client);
        }
        if (game.GetChosen())
        {
            game.DrawChosenWord(client);
        }
    }

    if (game.GetFinished())
    {
        // If admin, send FINISHED
        if (client->id == client->adminID)
        {
            string msg = "ROUND_OVER";
            client->Send(msg);
        }

        game.SetDefault();

        // Set painter to the next client in the list
        bool found = false;
        for (int i = 0; i < client->connectedClients.size(); i++)
        {
            if (found)
            {
                client->painterID = client->connectedClients[i].id;
                break;
            }

            if (client->connectedClients[i].id == client->painterID)
            {
                found = true;
            }
        }

        // Update if client is painter
        if (client->painterID == client->id)
        {
            game.SetIsGuesser(false);
        }
        else
        {
            game.SetIsGuesser(true);
        }

        currentRound++;
    }
}

void Partida::DrawRounds()
{
    if (currentRound != 3)
    {
        std::string roundText = "Ronda " + std::to_string(currentRound + 1) + " de 3";
        const char *textToDraw = roundText.c_str();
        DrawTextPro(GetFontDefault(), textToDraw, {120, 95}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {
        DrawTextPro(GetFontDefault(), "Partida finalizada", {58, 80}, {0, 0}, 0, 20, 4, BLACK);
    }
}
