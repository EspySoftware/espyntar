#pragma once
#include "./Painter.h"
#include "./Canvas.h"
#include "./ColorPalette.h"
#include "./Client.h"
#include "./WordServiceClient.h"
#include "./GameConfig.h"
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

    // Cliente para el servicio de palabras IA
    WordServiceClient wordService;
    string currentTheme;
    bool useWordService;

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
    const int drawTime = 140; // TIME TO DRAW
    const int setTime = 120;  // TIME TO CHOOSE A WORD
    Games(Painter &painter, Canvas &canvas, ColorPalette &palette, bool isGuesser);

    array<string, 3> GetRandomWords() const;
    array<string, 3> GetRandomWordsFromService(const string &theme) const;

    // Métodos para manejar temáticas
    void SetTheme(const string &theme) { currentTheme = theme; }
    string GetCurrentTheme() const { return currentTheme; }
    void SetUseWordService(bool use) { useWordService = use; }
    bool IsUsingWordService() const { return useWordService; }

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

    void StartTimer() { startTime = high_resolution_clock::now(); }                                                 // Start the timer
    int GetElapsedTime() const { return duration_cast<seconds>(high_resolution_clock::now() - startTime).count(); } // Get the elapsed time in seconds
};