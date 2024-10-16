#pragma once
#include "./Painter.h"
#include "./Canvas.h"
#include "./ColorPalette.h"
#include "./Client.h"
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <chrono>

using std::array;
using std::string;
using std::stringstream;
using std::vector;
using namespace std::chrono;

class Games
{
private:
    vector<string> words;
    array<string, 3> optionWords;

    string chosenWord;
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

    high_resolution_clock::time_point startTime; // Start time for the timer

public:
    const int drawTime = 80; // TIME TO DRAW
    const int setTime = 30; // TIME TO CHOOSE A WORD
    Games(Painter &painter, Canvas &canvas, ColorPalette &palette, bool isGuesser);

    array<string, 3> GetRandomWords() const;

    string GetChosenWord() const { return chosenWord; }
    void UpdateChosenWord(string word) { chosenWord = word; }

    void SetChosenWord(shared_ptr<Client> &client, Texture2D &clock);
    void DrawChosenWord(shared_ptr<Client> &client, Texture2D &clock);


    bool GetChosen() const { return chosen; }
    void SetIsGuesser(bool isGuesser) { this->isGuesser = isGuesser; }
    bool GetIsGuesser() { return isGuesser; }
    string CensorWord(string word);
    void DrawTimer(int &timer, Texture2D &clock);
    vector<string> FilterChat(vector<string> messages);
    bool GetFinished() { return finished; }
    void SetDefault();

    void StartTimer() { startTime = high_resolution_clock::now(); } // Start the timer
    int GetElapsedTime() const { return duration_cast<seconds>(high_resolution_clock::now() - startTime).count(); } // Get the elapsed time in seconds
};