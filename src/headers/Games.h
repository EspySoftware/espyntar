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
    bool isGuesser = false; // False means player is the drawer
    bool guessed = false;  // True means player has guessed the word
    Painter &painter;
    Canvas &canvas;
    ColorPalette &palette;
    shared_ptr<Client> client;

public:
    Games(Painter &painter, Canvas &canvas, ColorPalette &palette, shared_ptr<Client> client);
    array<string, 3> GetRandomWords() const;
    string GetChosenWord() const { return chosenWord; }
    void SetChosenWord();
    bool GetChosen() const { return chosen; }
    void DrawChosenWord();
    void SetIsGuesser(bool isGuesser) { this->isGuesser = isGuesser; }
    bool GetIsGuesser() { return isGuesser; }
    string CensorWord(string word);
    void DrawTimer(double timer);
    vector<string> FilterChat(vector<string> messages);
};