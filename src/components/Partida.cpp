#include "../headers/Partida.h"
#include "../headers/raygui.h"

Partida::Partida(Games &game, shared_ptr<Client> &client) : game(game)
{
}

void Partida::Ronda(shared_ptr<Client> &client)
{
    if (!started)
    {
        // Read "The game has started." from the messages
        for (int i = 0; i < client->messages.size(); i++)
        {
            if (client->messages[i] == "The game has started.")
            {
                SetMaxRounds(client->connectedClients.size() * 2);
                started = true;
                break;
            }
        }

        if (client->adminID == client->id)
        {
            DrawTextPro(GetFontDefault(), "Esperando jugadores...", {float(GetScreenWidth()) / 2 - MeasureText("Esperando jugadores...", 20) / 2, 80}, {0, 0}, 0, 20, 4, BLACK);
            if (GuiButton({(GetScreenWidth() / 2.0f) - 160, (GetScreenHeight() / 2.0f) - 80.0f, 320.0f, 70.0f}, "Iniciar partida"))
            {
                client->messages.push_back("The game has started.");

                // Send START message
                string msg = "START_GAME";
                client->Send(msg);
            }
        }
    }
    else
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
            game.SetDefault();

            // If admin, update painter ID to the next client (if last client, set painter to the first client)
            if (client->id == client->adminID)
            {
                for (int i = 0; i < client->connectedClients.size(); i++)
                {
                    // Find the painter ID and update it
                    if (client->connectedClients[i].id == client->painterID)
                    {
                        // If last client, set painter to the first client
                        if (i == client->connectedClients.size() - 1)
                        {
                            client->painterID = client->connectedClients[0].id;
                        }
                        // Else, set painter to the next client
                        else
                        {
                            client->painterID = client->connectedClients[i + 1].id;
                        }

                        // Send ROUND_OVER message
                        string msg = "ROUND_OVER";
                        client->Send(msg);

                        // Send new painter ID
                        msg = "PAINTER: " + std::to_string(client->painterID);
                        cout << "Sending new painter ID: " << msg << endl;
                        client->Send(msg);

                        break;
                    }
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
            if (game.GetFinished())
            {
                game.SetDefault();
                currentRound++;
            }
        }
    }
    DrawRounds();
}

void Partida::DrawRounds()
{
    if (!started)
    {
        DrawTextPro(GetFontDefault(), "Calculando rondas", {120, 80}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {
        if (currentRound < maxRounds)
        {
            std::string roundText = "Ronda " + std::to_string(currentRound + 1) + " de " + std::to_string(maxRounds);
            const char *textToDraw = roundText.c_str();
            DrawTextPro(GetFontDefault(), textToDraw, {120, 95}, {0, 0}, 0, 20, 4, BLACK);
        }
    }
}