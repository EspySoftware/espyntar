#ifndef PARTIDA_H
#define PARTIDA_H

#include "./Games.h"
#include "./Screen.h"
#include "./raylib.h"
#include <memory>
#include <string>

class Partida
{
private:
    int currentRound = 0;
    int maxRounds = 0;
    bool started = false;
    Games &game;

    // Métodos auxiliares
    void VerificarInicio(std::shared_ptr<Client> &client);
    void MostrarEsperandoJugadores(std::shared_ptr<Client> &client);
    void ManejarRonda(std::shared_ptr<Client> &client, Texture2D &clock);
    void DibujarTextoCentrado(const char *texto, float posicionY, int tamanoFuente, Color color);
    void DibujarCuadroTransparente();

public:
    Partida(Games &game, std::shared_ptr<Client> &client);
    
    void Ronda(std::shared_ptr<Client> &client, Screen *scene, Texture2D &clock);
    void DrawRounds(Screen *scene);

    // Getters y Setters
    bool GetStarted() const { return started; }
    Games &GetGame() { return game; }
    void SetMaxRounds(int maxRounds) { this->maxRounds = maxRounds; }
};

#endif // PARTIDA_H
