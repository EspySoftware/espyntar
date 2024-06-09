#pragma once
#include "./Painter.h"
#include "./Canvas.h"
#include "./ColorPalette.h"
#include "./Client.h"
#include <vector>
#include <array>
#include <string>
using std::array;
using std::string;
using std::vector;

class Games
{
private:
    vector<string> words;
    string chosenWord;
    bool chosen = false;
    bool isGuesser = true; // False means player is the drawer
    bool guessed = false;  // True means player has guessed the word
    Painter &painter;
    Canvas &canvas;
    ColorPalette &palette;

public:
    Games(Painter &painter, Canvas &canvas, ColorPalette &palette);
    array<string, 3> GetRandomWords() const;
    string GetChosenWord() const { return chosenWord; }
    void SetChosenWord();
    bool GetChosen() const { return chosen; }
    void DrawChosenWord(shared_ptr<Client> &client);
    void SetIsGuesser(bool isGuesser) { this->isGuesser = isGuesser; }
    bool GetIsGuesser() { return isGuesser; }
    string CensorWord(string word);
    void DrawTimer(int &timer);
    vector<string> FilterChat(vector<string> messages);
};