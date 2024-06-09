#include <fstream>
#include <iostream>
#include "../headers/Games.h"
#include "../headers/raygui.h"
#define FRAMES 144

using std::endl;
using std::getline;
using std::ifstream;

Games::Games(Painter &painter, Canvas &canvas, ColorPalette &palette) : painter(painter), canvas(canvas), palette(palette)
{
    string line;
    ifstream file("../assets/words.txt");
    while (getline(file, line))
    {
        words.push_back(line);
    }
    file.close();
    for (int i = 0; i < words.size(); i++)
    {
        for (int j = 0; j < words[i].size(); j++)
        {
            words[i][j] = toupper(words[i][j]);
        }
    }
}
array<string, 3> Games::GetRandomWords() const
{
    array<string, 3> three_word;
    for (int i = 0; i < 3; i++)
    {
        three_word[i] = words[rand() % words.size()];
    }
    return three_word;
}

void Games::SetChosenWord()
{
    static double timer = 10 * FRAMES;
    static array<string, 3> words = GetRandomWords();
    DrawTimer(timer);

    if (timer <= 0 && !chosen)
    {
        chosenWord = words[0];
        chosen = true;
    }
    painter.SetCanPaint(false);
    if (!isGuesser)
    {
        DrawTextPro(GetFontDefault(), "Selecciona:", {(GetScreenWidth() / 2.0f) - (MeasureText("Selecciona:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
        DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200}); // cuadro transparente
        if (GuiButton({(GetScreenWidth() / 2.0f) - 320, (GetScreenHeight() / 2.0f) - 80.0f, 160.0f, 70.0f}, words[0].c_str()))
        {
            chosenWord = words[0];
            chosen = true;
            painter.SetColor(22);
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) - 80, (GetScreenHeight() / 2.0f) - 80.0f, 160.0f, 70.0f}, words[1].c_str()))
        {
            chosenWord = words[1];
            chosen = true;
            painter.SetColor(22);
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) + 160, (GetScreenHeight() / 2.0f) - 80.0f, 160.0f, 70.0f}, words[2].c_str()))
        {
            chosenWord = words[2];
            chosen = true;
            painter.SetColor(22);
        }
    }
    else
    {
        DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200}); // cuadro transparente
        DrawTextPro(GetFontDefault(), "Esperando a NOMBRE-DE-JUGADOR", {(GetScreenWidth() / 2.0f) - (MeasureText("Esperando a NOMBRE-DE-JUGADOR", 20) / 2), GetScreenHeight() - 500.0f}, {0, 0}, 0, 20, 4, BLACK);
    }
}

void Games::DrawChosenWord(shared_ptr<Client> &client)
{
    static bool censored = false;
    static string censoredString = CensorWord(chosenWord);
    static bool guessed = false;
    static vector<string> filtered;

    static double timer = 80 * FRAMES;
    if (timer < 144)
    {
        censoredString = chosenWord;
        DrawTimer(0);
        if (timer < -(5 * FRAMES))
        {
            cout << timer << endl;
            return;
        }
    }
    else
    {
        DrawTimer(timer);
    }
    // Draw the number of letters in the word
    int i = censoredString.size();
    std::string str = std::to_string(i);
    DrawTextPro(GetFontDefault(), str.c_str(), {(GetScreenWidth() / 2.0f) + (MeasureText(censoredString.c_str(), 20)), 90}, {0, 0}, 0.0f, 10.0f, 2.0f, BLACK);

    // Draw the word
    if (!isGuesser)
    {
        painter.SetCanPaint(true);
        DrawTextPro(GetFontDefault(), "DIBUJA:", {(GetScreenWidth() / 2.0f) - (MeasureText("Dibuja:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
        DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
        canvas.DrawPalette(palette);
        // agregar paleta solo al que dibuja
    }
    else
    {
        if (!guessed)
        {
            painter.SetCanPaint(false);
            vector<string> messages = client->getMessages();
            filtered = FilterChat(messages);
            cout << chosenWord << endl;
            for (int i = 0; i < filtered.size(); i++)
            {
                if (filtered[i] == chosenWord) // modificar para sumar puntos (no hay cliente para sumar puntos aun)
                {
                    // connectedClients.at(0).AddPoints(1);
                    client->AddPoints(1);
                    guessed = true;
                }
            }
            DrawTextPro(GetFontDefault(), "ADIVINA:", {(GetScreenWidth() / 2.0f) - (MeasureText("Adivina:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);

            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
            if (timer <= (35 * FRAMES))
            {
                static int rand1 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand1) = chosenWord.at(rand1);
            }
            if (timer <= (50 * FRAMES))
            {
                static int rand2 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand2) = chosenWord.at(rand2);
            }
        }
        else
        {
            DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
            DrawTextPro(GetFontDefault(), "ADIVINADO", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINADO", 20) / 2), +300}, {0, 0}, 0, 20, 4, BLACK);
        }
    }
}

string Games::CensorWord(string word)
{
    static string censoredString = chosenWord;
    for (int i = 0; i < censoredString.size(); i++)
    {
        censoredString.at(i) = '_';
    }
    return censoredString;
}

void Games::DrawTimer(double &timer)
{
    DrawTextPro(GetFontDefault(), "TIEMPO:", {50, 80}, {0, 0}, 0, 20, 4, BLACK);
    if (timer > 0)
    {
        timer--;
    }
    DrawTextPro(GetFontDefault(), std::to_string(timer / 144).c_str(), {50, 100}, {0, 0}, 0, 20, 4, BLACK);
}

void Games::DrawTimer(int number)
{
    DrawTextPro(GetFontDefault(), "TIEMPO:", {50, 80}, {0, 0}, 0, 20, 4, BLACK);
    DrawTextPro(GetFontDefault(), std::to_string(number).c_str(), {50, 100}, {0, 0}, 0, 20, 4, BLACK);
}

vector<string> Games::FilterChat(vector<string> messages)
{
    vector<string> filteredMessages;
    for (int i = 0; i < messages.size(); i++)
    {
        for (int j = 0; j < messages[i].size(); j++)
        {
            messages[i][j] = toupper(messages[i][j]);
        }
        messages[i] = messages[i].substr(messages[i].find(":") + 1);
        messages[i].erase(0, 1);
        filteredMessages.push_back(messages[i]);
    }
    return filteredMessages;
}
