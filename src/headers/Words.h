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
    public:
        Words();
        array<string, 3> GetRandomWords() const;
        string GetChosenWord() const { return chosenWord; }
        void SetChosenWord(string word) { chosenWord = word; }
};