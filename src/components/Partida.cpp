#include "../headers/Partida.h"
#include "../headers/raygui.h"

Partida::Partida(Games &game, shared_ptr<Client> &client) 
    : game(game), started(false), currentRound(0), maxRounds(0) 
{
}

void Partida::Ronda(shared_ptr<Client> &client, Screen *scene, Texture2D &clock) 
{
    if (!started) 
    {
        VerificarInicio(client);
        MostrarEsperandoJugadores(client);
    } 
    else 
    {
        ManejarRonda(client, clock);
    }

    DrawRounds(scene);
}

void Partida::VerificarInicio(shared_ptr<Client> &client) 
{
    for (const auto &mensaje : client->messages) 
    {
        if (mensaje == "The game has started.") 
        {
            SetMaxRounds(client->connectedClients.size() * 2); // Una ronda por jugador
            started = true;
            return;
        }
    }
}

void Partida::MostrarEsperandoJugadores(shared_ptr<Client> &client) 
{
    if (client->adminID == client->id) 
    {
        DibujarTextoCentrado("Esperando jugadores...", 80, 20, BLACK);
        DibujarCuadroTransparente();

        if (GuiButton({(GetScreenWidth() / 2.0f) - 160, 400, 320.0f, 70.0f}, "Iniciar partida")) 
        {
            client->messages.push_back("The game has started.");
            client->Send("START_GAME");
        }
    }
}

void Partida::ManejarRonda(shared_ptr<Client> &client, Texture2D &clock) 
{
    if (currentRound <= maxRounds) 
    {
        if (!game.GetChosen()) 
        {
            game.SetChosenWord(client, clock);
        } 
        else 
        {
            game.DrawChosenWord(client, clock);
        }
    }

    if (game.GetFinished() || client->round_over) 
    {
        game.SetDefault();
        currentRound++;
        client->round_over = false;
    }
}

void Partida::DrawRounds(Screen *scene) 
{
    if (!started) 
    {
        DibujarTextoCentrado("Calculando rondas", 80, 20, BLACK);
    } 
    else 
    {
        if (currentRound == maxRounds) 
        {
            DibujarTextoCentrado("Ronda extra!", 80, 20, BLACK);
            scene->scene = WINNER;
        } 
        else 
        {
            std::string textoRonda = "Ronda " + std::to_string(currentRound + 1) + " de " + std::to_string(maxRounds);
            DibujarTextoCentrado(textoRonda.c_str(), 95, 20, BLACK);
        }
    }
}

// Métodos auxiliares
void Partida::DibujarTextoCentrado(const char *texto, float posicionY, int tamanoFuente, Color color) 
{
    DrawTextPro(GetFontDefault(), texto, 
                {float(GetScreenWidth()) / 2 - MeasureText(texto, tamanoFuente) / 2, posicionY}, 
                {0, 0}, 0, tamanoFuente, 4, color);
}

void Partida::DibujarCuadroTransparente() 
{
    DrawRectangle(GetScreenWidth() / 2 - 700 / 2, 
                  GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 
                  700, 560, {102, 149, 89, 200});
}
