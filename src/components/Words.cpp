#include <fstream>
#include <iostream>
#include "../headers/Words.h"
#include "../headers/raygui.h"
#define FRAMES 144

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
    static double timer = 0;
    static array<string, 3> words = GetRandomWords();
    std::cout << timer << std::endl;
    timer += 1;
    DrawTimer(timer);
    if (timer == (15 * FRAMES))
    {
        chosenWord = words[0];
        chosen = true;
        return;
    }
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

    static bool censored = false;
    static double timer = 0;
    static string censoredString = CensorWord(chosenWord);
    timer += 1;
    DrawTimer(timer);
    // Draw the word
    if (!isPlayer)
    {
        DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +70}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {
        if (timer >= (80 * FRAMES))
        {
            censoredString = chosenWord;
            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +70}, {0, 0}, 0, 20, 4, BLACK);
        }
        else
        {
            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +70}, {0, 0}, 0, 20, 4, BLACK);
            if (timer >= (35 * FRAMES))
            {
                static int rand1 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand1) = chosenWord.at(rand1);
            }
            if (timer >= (50 * FRAMES))
            {
                static int rand2 = rand() % chosenWord.size();
                censoredString.at(rand2) = chosenWord.at(rand2);
            }
            if (timer >= (65 * FRAMES))
            {
                static int rand3 = rand() % chosenWord.size();
                censoredString.at(rand3) = chosenWord.at(rand3);
            }
        }
    }
}

string Words::CensorWord(string word)
{
    static string censoredString = chosenWord;
    for (int i = 0; i < censoredString.size(); i++)
    {
        censoredString.at(i) = '_';
    }
    return censoredString;
}

void Words::DrawTimer(double timer)
{
    DrawTextPro(GetFontDefault(), "Tiempo:", {float(GetScreenWidth()) - 200, 80}, {0, 0}, 0, 20, 4, BLACK);
    DrawTextPro(GetFontDefault(), std::to_string(timer / 144).c_str(), {float(GetScreenWidth()) - 200, 100}, {0, 0}, 0, 20, 4, BLACK);
} 
