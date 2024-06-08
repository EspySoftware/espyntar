#include <fstream>
#include <iostream>
#include "../headers/Words.h"
#include "../headers/raygui.h"

using std::endl;
using std::getline;
using std::ifstream;

Words::Words()
{
    string line;
    ifstream file("../assets/words.txt");
    while (getline(file, line))
    {
        words.push_back(line);
    }
    file.close();
}
array<string, 3> Words::GetRandomWords() const
{
    array<string, 3> three_words;
    for (int i = 0; i < 3; i++)
    {
        three_words[i] = words[rand() % words.size()];
    }
    return three_words;
}
void Words::SetChosenWord()
{
    static array<string, 3> words = GetRandomWords();
    if (GuiButton({(GetScreenWidth() / 2.0f) - 220, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[0].c_str()))
    {
        chosenWord = words[0];
        chosen = true;
    }
    if (GuiButton({(GetScreenWidth() / 2.0f) - 60, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[1].c_str()))
    {
        chosenWord = words[1];
        chosen = true;
    }
    if (GuiButton({(GetScreenWidth() / 2.0f) + 100, GetScreenHeight() - 500.0f, 120.0f, 50.0f}, words[2].c_str()))
    {
        chosenWord = words[2];
        chosen = true;
    }
}
void Words::DrawChosenWord()
{
    // Draw the word
    if (!isPlayer)
    {
        DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +70}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {
        for (int i = 0; i < chosenWord.size(); i++)
        {
            DrawRectangle((GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20)) + i * 30 - 15, 90.0f, 20.0f, 5.0f, BLACK);
        }
    }
}