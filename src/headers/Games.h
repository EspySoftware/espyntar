#pragma once
#include "./Painter.h"
#include "./Canvas.h"
#include "./ColorPalette.h"
#include "./Client.h"
#include <vector>
#include <array>
#include <string>
#include <sstream>
#define FRAMES 60

using std::array;
using std::string;
using std::stringstream;
using std::vector;

class Games
{
private:
    vector<string> words;
    array<string, 3> optionWords;
    string chosenWord;
    string prevChosenWord;

    string censoredString;
    bool censored = false;
    bool chosen = false;
    bool isFiltered = false;
    bool isGuesser = true; // False means player is the drawer
    bool guessed = false;  // True means player has guessed the word
    bool finished = false;
    Painter &painter;
    Canvas &canvas;
    ColorPalette &palette;

public:
    int drawTimer = 80 * FRAMES; // TIME TO DRAW
    int setTimer = 30 * FRAMES; // TIME TO CHOOSE A WORD
    Games(Painter &painter, Canvas &canvas, ColorPalette &palette, bool isGuesser);

    array<string, 3> GetRandomWords() const;
    string GetChosenWord() const { return chosenWord; }
    void SetChosenWord(shared_ptr<Client> &client, Texture2D &clock);
    void UpdateChosenWord(string word) { chosenWord = word; }

    bool GetChosen() const { return chosen; }

    void DrawChosenWord(shared_ptr<Client> &client, Texture2D &clock);
    void SetIsGuesser(bool isGuesser) { this->isGuesser = isGuesser; }
    bool GetIsGuesser() { return isGuesser; }
    string CensorWord(string word);
    void DrawTimer(int &timer, Texture2D &clock);
    vector<string> FilterChat(vector<string> messages);
    bool GetFinished() { return finished; }
    void SetDefault();
    int &GetDrawTimer() { return drawTimer; }
};