#include <fstream>
#include <iostream>
#include <sstream>
#include <random>
#include <raylib.h>
#include <raygui.h>
#include "../headers/Round.h"

#define FRAMES 60

using std::array;
using std::endl;
using std::ifstream;
using std::shared_ptr;
using std::string;
using std::stringstream;
using std::vector;

// Constructor: Inicializa palabras y convierte a mayúsculas
Round::Round(Painter &painter, Canvas &canvas, ColorPalette &palette)
    : painter(painter), canvas(canvas), palette(palette) {
    LoadWordsFromFile("../assets/words.txt");
    ConvertWordsToUpper();
    optionWords = GetRandomWords();
    SetDefault();
}

// Carga palabras desde archivo
void Round::LoadWordsFromFile(const string &filePath) {
    ifstream file(filePath);
    string line;
    while (getline(file, line)) {
        words.push_back(line);
    }
    file.close();
}

// Convierte palabras a mayúsculas
void Round::ConvertWordsToUpper() {
    for (auto &word : words) {
        for (auto &c : word) {
            c = toupper(c);
        }
    }
}

// Obtiene 3 palabras aleatorias
array<string, 3> Round::GetRandomWords() const {
    array<string, 3> threeWords;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, words.size() - 1);

    for (auto &word : threeWords) {
        word = words[dist(gen)];
    }
    return threeWords;
}

// Establece la palabra elegida y envía el mensaje al cliente
void Round::SetChosenWord(shared_ptr<Client> &client) {
    DrawTimer(setTimer);

    if (setTimer <= 0 && !chosen) {
        ChooseWord(client, optionWords[0]);
    }

    painter.SetCanPaint(false);
    canvas.Clear();

    if (!isGuesser) {
        DrawChooseWordScreen(client);
    } else {
        DrawWaitingScreen();
    }
}

// Pantalla de selección de palabra
void Round::DrawChooseWordScreen(shared_ptr<Client> &client) {
    DrawTextCentered("Selecciona:", 25, 60.0f, BLACK);
    DrawTransparentBox();

    for (int i = 0; i < optionWords.size(); ++i) {
        if (GuiButton(GetButtonRect(i), optionWords[i].c_str())) {
            ChooseWord(client, optionWords[i]);
        }
    }
}

// Manejo del botón seleccionado
Rectangle Round::GetButtonRect(int index) const {
    float baseX = (GetScreenWidth() / 2.0f) - 320.0f;
    float baseY = (GetScreenHeight() / 2.0f) - 80.0f;
    return {baseX + index * 240.0f, baseY, 160.0f, 70.0f};
}

// Selección de palabra y envío al cliente
void Round::ChooseWord(shared_ptr<Client> &client, const string &word) {
    chosenWord = word;
    censoredString = CensorWord(chosenWord);

    stringstream msg;
    msg << "ANSWER: " << chosenWord;
    client->Send(msg.str());

    chosen = true;
    painter.SetColor(22);
}

// Pantalla de espera
void Round::DrawWaitingScreen() const {
    DrawTransparentBox();
    DrawTextCentered("Esperando a NOMBRE-DE-JUGADOR", 20, GetScreenHeight() - 500.0f, BLACK);
}

// Dibuja la palabra elegida y maneja el temporizador
void Round::DrawChosenWord(shared_ptr<Client> &client) {
    if (drawTimer < 60 || client->round_over) {
        EndRound(client);
    } else {
        DrawTimer(drawTimer);
    }

    if (!isGuesser) {
        DrawPainterScreen();
    } else {
        DrawGuesserScreen(client);
    }
}

// Finaliza la ronda
void Round::EndRound(shared_ptr<Client> &client) {
    client->round_over = false;
    painter.SetCanPaint(false);
    censoredString = chosenWord;
    drawTimer--;

    if (drawTimer < -(5 * FRAMES)) {
        finished = true;
    }
}

// Dibuja la pantalla del pintor
void Round::DrawPainterScreen() {
    if (drawTimer > 60) painter.SetCanPaint(true);

    DrawTextCentered("DIBUJA:", 25, 60.0f, BLACK);
    DrawTextCentered(chosenWord, 20, 100.0f, BLACK);
    canvas.DrawPalette(palette);
}

// Dibuja la pantalla del adivinador
void Round::DrawGuesserScreen(shared_ptr<Client> &client) {
    if (!guessed) {
        painter.SetCanPaint(false);
        HandleGuesses(client);
        DrawTextCentered("ADIVINA:", 25, 60.0f, BLACK);
        DrawTextCentered(censoredString, 20, 100.0f, BLACK);
        RevealLetters();
    } else {
        DrawTextCentered("ADIVINA:", 25, 60.0f, BLACK);
        DrawTextCentered(chosenWord, 20, 100.0f, BLACK);
        DrawTextCentered("ADIVINADO", 20, 300.0f, BLACK);
    }
}

// Maneja las suposiciones de palabras
void Round::HandleGuesses(shared_ptr<Client> &client) {
    vector<string> messages = FilterChat(client->getMessages());
    for (const auto &msg : messages) {
        if (msg == chosenWord) {
            client->AddPoints(1);
            guessed = true;
        }
    }
}

// Censura la palabra
string Round::CensorWord(const string &word) const {
    return string(word.size(), '_');
}

// Revela letras progresivamente
void Round::RevealLetters() {
    if (drawTimer == (35 * FRAMES) || drawTimer == (50 * FRAMES)) {
        int randIndex = rand() % static_cast<int>(chosenWord.size());
        censoredString.at(randIndex) = chosenWord.at(randIndex);
    }
}

// Dibuja el temporizador
void Round::DrawTimer(int &timer) {
    if (timer > 0) timer--;

    Rectangle backgroundRect = {110, 105, 40, 30};
    DrawRectangleRec(backgroundRect, Fade(GRAY, 0.5f));
    DrawTextCentered(std::to_string(timer / 60), 20, 110.0f, BLACK);
}

// Filtra el chat y extrae mensajes en mayúsculas
vector<string> Round::FilterChat(const vector<string> &messages) const {
    vector<string> filtered;
    for (const auto &msg : messages) {
        string upperMsg = msg.substr(msg.find(":") + 1);
        for (auto &c : upperMsg) c = toupper(c);
        filtered.push_back(upperMsg);
    }
    return filtered;
}

// Configuración por defecto
void Round::SetDefault() {
    chosen = guessed = censored = finished = isFiltered = false;
    drawTimer = 80 * FRAMES;
    setTimer = 30 * FRAMES;
    optionWords = GetRandomWords();
}

// Dibuja un cuadro transparente
void Round::DrawTransparentBox() const {
    DrawRectangle(GetScreenWidth() / 2 - 350, GetScreenHeight() / 2 - 280, 700, 560, {102, 149, 89, 200});
}

// Dibuja texto centrado
void Round::DrawTextCentered(const string &text, int fontSize, float y, Color color) const {
    float x = (GetScreenWidth() - MeasureText(text.c_str(), fontSize)) / 2.0f;
    DrawText(text.c_str(), x, y, fontSize, color);
}
