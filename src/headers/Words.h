#pragma once
#include "./Painter.h"
#include "./Canvas.h"
#include "./ColorPalette.h"
#include <vector>
#include <array>
#include <string>
using std::vector;
using std::array;
using std::string;

class Words
{
    private:
        vector<string> words;
        string chosenWord;
        bool chosen = false;
        bool isGuesser = false; // False means player is the drawer
        Painter &painter;
        Canvas &canvas;
        ColorPalette &palette;
    public:
        Words(Painter &painter, Canvas &canvas, ColorPalette &palette);
        array<string, 3> GetRandomWords() const;
        string GetChosenWord() const { return chosenWord; }
        void SetChosenWord();
        bool GetChosen() const {return chosen; }
        void DrawChosenWord();
        void SetisGuesser(bool isGuesser) {this->isGuesser = isGuesser; }
        bool GetisGuesser() {return isGuesser; }
        string CensorWord(string word);
        void DrawTimer(double timer);
};