#pragma once
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
        bool isPlayer;
    public:
        Words();
        array<string, 3> GetRandomWords() const;
        string GetChosenWord() const { return chosenWord; }
        void SetChosenWord();
        bool GetChosen() const {return chosen; }
        void DrawChosenWord();
        void SetIsPlayer(bool isPlayer) {this->isPlayer = isPlayer; }
};