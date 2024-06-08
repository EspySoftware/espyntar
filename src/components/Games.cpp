#include <fstream>
#include <iostream>
#include "../headers/Games.h"
#include "../headers/raygui.h"
#define FRAMES 144

using std::endl;
using std::getline;
using std::ifstream;

Games::Games(Painter &painter, Canvas &canvas, ColorPalette &palette, shared_ptr<Client> client) : painter(painter), canvas(canvas), palette(palette), client(client)
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
    static int timer = 10 * FRAMES;
    static array<string, 3> words = GetRandomWords();
    DrawTimer(timer);

    if (timer <= 0 && !chosen)
    {
        chosenWord = words[0];
        chosen = true;
    }

    if (!isGuesser)
    {
        painter.SetBrushSize(0.0f);
        if (GuiButton({(GetScreenWidth() / 2.0f) - 220, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[0].c_str()))
        {
            chosenWord = words[0];
            chosen = true;
            painter.SetColor(22);
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[1].c_str()))
        {
            chosenWord = words[1];
            chosen = true;
            painter.SetColor(22);
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) + 100, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[2].c_str()))
        {
            chosenWord = words[2];
            chosen = true;
            painter.SetColor(22);
        }
    }
    else
    {
        painter.SetBrushSize(0.0f);
        DrawTextPro(GetFontDefault(), "Esperando a NOMBRE-DE-JUGADOR", {(GetScreenWidth() / 2.0f) - (MeasureText("Esperando a NOMBRE-DE-JUGADOR", 20) / 2), GetScreenHeight() - 500.0f}, {0, 0}, 0, 20, 4, BLACK);
    }
}

void Games::DrawChosenWord() // mandarle como referencia al cliente
{
    static bool censored = false;
    static string censoredString = CensorWord(chosenWord);
    static vector<string> filtered;

    static int timer = 120 * FRAMES;
    DrawTimer(timer);

    vector<string> messages = client->getMessages();
    filtered = FilterChat(messages);
    for (int i = 0; i < filtered.size(); i++)
    {
        if (filtered[i] == chosenWord)
        {
            DrawTextPro(GetFontDefault(), "ADIVINADO", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINADO", 20) / 2), +300}, {0, 0}, 0, 20, 4, BLACK);
            client->points += 100;
            guessed = true;
        }
    }

    // Draw the word
    if (!isGuesser)
    {
        int i = censoredString.size();
        std::string str = std::to_string(i);
        DrawTextPro(GetFontDefault(), str.c_str(), {(GetScreenWidth() / 2.0f) + (MeasureText(censoredString.c_str(), 20) / 2), 90}, {0, 0}, 0.0f, 10.0f, 2.0f, BLACK);
        DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {

        painter.SetBrushSize(0.0f);
        if (timer >= (80 * FRAMES))
        {
            censoredString = chosenWord;
            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
        }
        else
        {
            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
            if (timer >= (35 * FRAMES))
            {
                static int rand1 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand1) = chosenWord.at(rand1);
            }
            if (timer >= (50 * FRAMES))
            {
                static int rand2 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand2) = chosenWord.at(rand2);
            }
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

void Games::DrawTimer(int &timer)
{
    DrawTextPro(GetFontDefault(), "Tiempo:", {50, 80}, {0, 0}, 0, 20, 4, BLACK);
    if (timer > 0)
    {
        timer--;
    }
    DrawTextPro(GetFontDefault(), std::to_string(timer / 144).c_str(), {50, 100}, {0, 0}, 0, 20, 4, BLACK);
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
