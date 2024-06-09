#include "../headers/Partida.h"

Partida::Partida(Games &game) : game(game)
{
    this->musicOn = true;
    this->effects = true;
    this->background = LoadTexture("../assets/background.png");
    this->title = LoadTexture("../assets/espy.png");
}
void Partida::DrawConfig()
{
    BeginDrawing();

    DrawTexture(this->background, 0, 0, WHITE);
    DrawTexture(this->title, (GetScreenWidth() - this->title.width) / 2, 50, WHITE);

    Color color1 = {102, 149, 89, 200};
    Rectangle rec = {
        (GetScreenWidth() / 2.0f) - 175.0f,
        (GetScreenHeight() / 2.0f) - 150.0f,
        350.0f,
        400.0f};
    DrawRectangleRounded(rec, 0.5f, 0.5, color1);

    // Posición central del rectángulo
    float rectCenterX = rec.x + (rec.width / 2.0f);
    float rectCenterY = rec.y + (rec.height / 2.0f);
    float buttonWidth = 170.0f;
    float buttonHeight = 50.0f;
    float spacing = 20.0f; // Espacio entre botones

    // Calcular posiciones Y de los botones de manera que estén centrados
    float totalHeight = (buttonHeight * 4) + (spacing * 3);
    float startY = rectCenterY - (totalHeight / 2.0f);

    // // Botón de Música
    // buttons(rectCenterX - (buttonWidth / 2.0f), startY, buttonWidth, buttonHeight, "MUSICA");
    // if (this->musicOn) // musica activada
    // {
    //     if (GuiButton({rectCenterX - (buttonWidth / 2.0f), startY, buttonWidth, buttonHeight}, "MUSICA on"))
    //     {
    //         this->musicOn = false; // desactivar musica
    //     }
    // }
    // else // musica desactivada
    // {
    //     if (GuiButton({rectCenterX - (buttonWidth / 2.0f), startY, buttonWidth, buttonHeight}, "MUSICA off"))
    //     {
    //         this->musicOn = true; // activar musica
    //     }
    // }

    // // Botón de Efectos
    // buttons(rectCenterX - (buttonWidth / 2.0f), startY + buttonHeight + spacing, buttonWidth, buttonHeight, "EFECTOS");
    // if (this->effects) // efectos activados
    // {
    //     if (GuiButton({rectCenterX - (buttonWidth / 2.0f), startY + buttonHeight + spacing, buttonWidth, buttonHeight}, "EFECTOS on"))
    //     {
    //         this->effects = false; // desactivar efectos
    //     }
    // }
    // else // efectos desactivados
    // {
    //     if (GuiButton({rectCenterX - (buttonWidth / 2.0f), startY + buttonHeight + spacing, buttonWidth, buttonHeight}, "EFECTOS off"))
    //     {
    //         this->effects = true; // activar efectos
    //     }
    // }

    // // Botón Volver
    // buttons(rectCenterX - (buttonWidth / 2.0f), startY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight, "VOLVER");
    // if (GuiButton({rectCenterX - (buttonWidth / 2.0f), startY + 2 * (buttonHeight + spacing), buttonWidth, buttonHeight}, "VOLVER"))
    // {
    //     this->scene.scene = GAME;
    // }

    // // Botón Salir
    // buttons(rectCenterX - (buttonWidth / 2.0f), startY + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight, "SALIR");
    // if (GuiButton({rectCenterX - (buttonWidth / 2.0f), startY + 3 * (buttonHeight + spacing), buttonWidth, buttonHeight}, "SALIR"))
    // {
    //     this->scene.scene = EXIT;
    // }

    EndDrawing();
}

void Partida::Ronda(shared_ptr<Client> &client)
{
    if (currentRound <= maxRounds)
    {
        game.UpdateChosenWord(client->chosenWord);

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
