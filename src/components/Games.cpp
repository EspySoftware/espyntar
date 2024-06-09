#include <fstream>
#include <iostream>
#include "../headers/Games.h"
#include "../headers/raygui.h"

#define FRAMES 144
#define BASE_POINTS 100

using std::endl;
using std::getline;
using std::ifstream;

Games::Games(Painter &painter, Canvas &canvas, ColorPalette &palette, bool isGuesser) : painter(painter), canvas(canvas), palette(palette)
{
    this->isGuesser = isGuesser;
    string line;
    ifstream file("../assets/words.txt");
    while (getline(file, line))
    {
        words.push_back(line);
    }
    file.close();
    for (int i = 0; i < words.size(); i++)
    {
        for (int j = 0; j < words[i].size(); j++)
        {
            words[i][j] = toupper(words[i][j]);
        }
    }
    optionWords = GetRandomWords();
}

array<string, 3> Games::GetRandomWords() const
{
    array<string, 3> three_word;
    for (int i = 0; i < 3; i++)
    {
        three_word[i] = words[rand() % words.size()];
    }
    return three_word;
}

void Games::SetChosenWord(shared_ptr<Client> &client)
{
    stringstream msg;

    DrawTimer(setTimer);

    // Parse messages for "Word has been chosen." message
    vector<string> messages = client->getMessages();
    for (int i = 0; i < messages.size(); i++)
    {
        if (messages[i] == "Word has been chosen.")
        {
            setTimer = 0;

            // Remove message from messages
            client->messages.erase(client->messages.begin() + i);
            break;
        }
    }

    if (isGuesser && setTimer <= 0 && !chosen)
    {
        chosenWord = client->chosenWord;
        censoredString = CensorWord(chosenWord);

        chosen = true;
        painter.SetColor(22);
    }

    painter.SetCanPaint(false);
    canvas.Clear();
    if (!isGuesser)
    {
        if (setTimer <= 0 && !chosen)
        {
            chosenWord = optionWords[0];
            censoredString = CensorWord(chosenWord);

            // Broadcast chosen word
            msg << "ANSWER: " << chosenWord;
            client->Send(msg.str());

            chosen = true;
            painter.SetColor(22);
        }

        DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200}); // cuadro transparente
        DrawTextPro(GetFontDefault(), "Selecciona:", {(GetScreenWidth() / 2.0f) - (MeasureText("Selecciona:", 35) / 2), (GetScreenWidth() / 2.0f) - 350}, {0, 0}, 0.0f, 35, 3.0f, BLACK);
        if (GuiButton({(GetScreenWidth() / 2.0f) - 320, 400, 160.0f, 70.0f}, optionWords[0].c_str()))
        {
            chosenWord = optionWords[0];
            censoredString = CensorWord(chosenWord);

            // Broadcast chosen word
            msg << "ANSWER: " << chosenWord;
            client->Send(msg.str());

            chosen = true;
            painter.SetColor(22);
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) - 80, 400, 160.0f, 70.0f}, optionWords[1].c_str()))
        {
            chosenWord = optionWords[1];
            censoredString = CensorWord(chosenWord);

            // Broadcast chosen word
            cout << chosenWord << endl;
            msg << "ANSWER: " << chosenWord;
            client->Send(msg.str());

            chosen = true;
            painter.SetColor(22);
        }
        if (GuiButton({(GetScreenWidth() / 2.0f) + 160, 400, 160.0f, 70.0f}, optionWords[2].c_str()))
        {
            chosenWord = optionWords[2];
            censoredString = CensorWord(chosenWord);

            // Broadcast chosen word
            cout << chosenWord << endl;
            msg << "ANSWER: " << chosenWord;
            client->Send(msg.str());
            chosen = true;
            painter.SetColor(22);
        }
    }
    else
    {
        // Look for painter name
        string adminName;
        for (int i = 0; i < client->connectedClients.size(); i++)
        {
            if (client->connectedClients[i].id == client->painterID)
            {
                adminName = client->connectedClients[i].name;
                break;
            }
        }

        string esperando = "Esperando a " + adminName;

        DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200}); // cuadro transparente
        DrawTextPro(GetFontDefault(), esperando.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(esperando.c_str(), 20) / 2), GetScreenHeight() - 500.0f}, {0, 0}, 0, 20, 4, BLACK);
    }
}

void Games::DrawChosenWord(shared_ptr<Client> &client)
{
    if (drawTimer < 144)
    {
        painter.SetCanPaint(false);
        censoredString = chosenWord;
        DrawTextPro(GetFontDefault(), "0", {55, 95}, {0, 0}, 0, 20, 4, BLACK);
        drawTimer--;
        if (drawTimer < -(5 * FRAMES))
        {
            finished = true; // Ends round
            return;
        }
    }
    else
    {
        DrawTimer(drawTimer);
    }

    // Draw the number of letters in the word
    int i = censoredString.size();
    std::string str = std::to_string(i);
    DrawTextPro(GetFontDefault(), str.c_str(), {(GetScreenWidth() / 2.0f) + (MeasureText(censoredString.c_str(), 20)), 90}, {0, 0}, 0.0f, 10.0f, 2.0f, BLACK);

    // Draw the word
    if (!isGuesser)
    {
        if (drawTimer > 144)
            painter.SetCanPaint(true);
        DrawTextPro(GetFontDefault(), "DIBUJA:", {(GetScreenWidth() / 2.0f) - (MeasureText("Dibuja:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
        DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
        canvas.DrawPalette(palette);
    }
    else
    {
        if (!guessed)
        {
            // cout << chosenWord << endl;
            painter.SetCanPaint(false);
            vector<string> messages = client->getMessages();
            vector<string> filtered = FilterChat(messages);
            for (int i = 0; i < filtered.size(); i++)
            {
                if (filtered[i] == chosenWord) // modificar para sumar puntos (no hay cliente para sumar puntos aun)
                {
                    if (guesses == 0)
                    {
                        client->AddPoints(BASE_POINTS);
                        guesses++;
                    }
                    else if (guesses != 0)
                        client->AddPoints(BASE_POINTS - (drawTimer / 144) - (guesses * 10));
                    guessed = true;
                }
            }
            DrawTextPro(GetFontDefault(), "ADIVINA:", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINA:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(censoredString.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
            if (drawTimer == (35 * FRAMES))
            {
                int rand1 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand1) = chosenWord.at(rand1);
            }
            if (drawTimer == (50 * FRAMES))
            {
                int rand2 = rand() % static_cast<int>(chosenWord.size());
                censoredString.at(rand2) = chosenWord.at(rand2);
            }
        }
        else
        {
            DrawTextPro(GetFontDefault(), "ADIVINA:", {(GetScreenWidth() / 2.0f) - (MeasureText("Adivina:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
            DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
            DrawTextPro(GetFontDefault(), "ADIVINADO", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINADO", 20) / 2), +300}, {0, 0}, 0, 20, 4, BLACK);
        }
    }
}

string Games::CensorWord(string word)
{
    if (!censored)
    {
        string toCensorString = chosenWord;
        for (int i = 0; i < toCensorString.size(); i++)
        {
            toCensorString.at(i) = '_';
        }
        return toCensorString;
    }
}

void Games::DrawTimer(int &timer)
{
    if (timer > 0)
    {
        timer--;
    }
    DrawTextPro(GetFontDefault(), std::to_string(timer / 144).c_str(), {55, 95}, {0, 0}, 0, 20, 4, BLACK);
}

vector<string> Games::FilterChat(vector<string> messages)
{
    vector<string> filteredMessages;
    for (int i = 0; i < messages.size(); i++)
    {
        for (int j = 0; j < messages[i].size(); j++)
        {
            messages[i][j] = toupper(messages[i][j]);
        }
        messages[i] = messages[i].substr(messages[i].find(":") + 1);
        messages[i].erase(0, 1);
        filteredMessages.push_back(messages[i]);
    }
    return filteredMessages;
}

void Games::SetDefault()
{
    chosen = false;
    guessed = false;
    censored = false;
    finished = false;
    isFiltered = false;
    guesses = 0;
    drawTimer = 10 * FRAMES;
    setTimer = 10 * FRAMES;
    optionWords = GetRandomWords();
}
