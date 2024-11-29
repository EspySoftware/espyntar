#include <fstream>
#include <iostream>
#include <unordered_set>
#include "../headers/Games.h"
#include "../headers/raygui.h"

#define BASE_POINTS 200
using std::endl;
using std::getline;
using std::ifstream;

Games::Games(Painter &painter, Canvas &canvas, ColorPalette &palette, bool isGuesser) 
    : painter(painter), canvas(canvas), palette(palette), isGuesser(isGuesser)
{
    ifstream file("../assets/words.txt");
    string line;
    while (getline(file, line))
    {
        for (char &c : line)
        {
            c = toupper(c);
        }
        words.push_back(line);
    }
    optionWords = GetRandomWords();
}

array<string, 3> Games::GetRandomWords() const
{
    array<string, 3> three_word;
    for (string &word : three_word)
    {
        word = words[rand() % words.size()];
    }
    return three_word;
}

void Games::SetChosenWord(shared_ptr<Client> &client, Texture2D &clock)
{
    stringstream msg;
    client->guessed = false;

    int timeRemaining = (setTime - GetElapsedTime());
    DrawTimer(timeRemaining, clock);

    ParseMessagesForChosenWord(client);

    if (isGuesser && GetElapsedTime() >= setTime && !chosen)
    {
        SetChosenWordForGuesser(client);
    }

    painter.SetCanPaint(false);
    canvas.Clear();

    if (!isGuesser)
    {
        HandlePainter(client, msg);
    }
    else
    {
        HandleGuesser(client);
    }
}

void Games::ParseMessagesForChosenWord(shared_ptr<Client> &client)
{
    vector<string> messages = client->getMessages();
    for (int i = 0; i < messages.size(); i++)
    {
        if (messages[i] == "Word has been chosen.")
        {
            client->messages.erase(client->messages.begin() + i);
            chosenWord = client->chosenWord;
            censoredString = CensorWord(chosenWord);
            chosen = true;
            break;
        }
    }
}

void Games::SetChosenWordForGuesser(shared_ptr<Client> &client)
{
    chosenWord = client->chosenWord;
    censoredString = CensorWord(chosenWord);
    chosen = true;
    painter.SetColor(22);
}

void Games::HandlePainter(shared_ptr<Client> &client, stringstream &msg)
{
    if (client->id == client->painterID)
    {
        SetIsGuesser(false);
        painter.SetCanPaint(true);
    }
    else
    {
        SetIsGuesser(true);
        painter.SetCanPaint(false);
    }

    if (GetElapsedTime() >= setTime && !chosen)
    {
        chosenWord = optionWords[0];
        client->chosenWord = chosenWord;
        censoredString = CensorWord(chosenWord);

        msg << "ANSWER: " << chosenWord;
        client->Send(msg.str());

        chosen = true;
        painter.SetColor(22);
    }

    DrawSelectionButtons(client, msg);
}

void Games::DrawSelectionButtons(shared_ptr<Client> &client, stringstream &msg)
{
    DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200});
    DrawTextPro(GetFontDefault(), "Selecciona:", {(GetScreenWidth() / 2.0f) - (MeasureText("Selecciona:", 35) / 2), (GetScreenWidth() / 2.0f) - 350}, {0, 0}, 0.0f, 35, 3.0f, BLACK);

    for (int i = 0; i < 3; i++)
    {
        if (GuiButton({(GetScreenWidth() / 2.0f) - 320 + (i * 240), 400, 160.0f, 70.0f}, optionWords[i].c_str()))
        {
            chosenWord = optionWords[i];
            client->chosenWord = chosenWord;
            censoredString = CensorWord(chosenWord);

            msg << "ANSWER: " << chosenWord;
            client->Send(msg.str());

            chosen = true;
            painter.SetColor(22);
        }
    }
}

void Games::HandleGuesser(shared_ptr<Client> &client)
{
    if (client->id == client->painterID)
    {
        SetIsGuesser(false);
        painter.SetCanPaint(true);
    }
    else
    {
        SetIsGuesser(true);
        painter.SetCanPaint(false);
    }

    string painterName = GetPainterName(client);
    string esperando = "Esperando a " + painterName;

    DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200});
    DrawTextPro(GetFontDefault(), esperando.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(esperando.c_str(), 20) / 2), GetScreenHeight() - 500.0f}, {0, 0}, 0, 20, 4, BLACK);
}

string Games::GetPainterName(shared_ptr<Client> &client)
{
    for (int i = 0; i < client->connectedClients.size(); i++)
    {
        if (client->connectedClients[i].id == client->painterID)
        {
            return client->connectedClients[i].name;
        }
    }
    return "";
}

void Games::DrawChosenWord(shared_ptr<Client> &client, Texture2D &clock)
{
    string prevChosenWord;
    int timeRemaining = (drawTime - GetElapsedTime());
    static bool messagesSent = false;

    CheckRoundOver(client);

    if (GetElapsedTime() >= drawTime)
    {
        client->round_over = true;
    }

    if (client->round_over)
    {
        EndRound(client, prevChosenWord, messagesSent);
    }
    else
    {
        timeRemaining = (drawTime - GetElapsedTime());
        DrawTimer(timeRemaining, clock);
    }

    DrawWord(client, timeRemaining);
}

void Games::CheckRoundOver(shared_ptr<Client> &client)
{
    if (client->id == client->adminID)
    {
        client->round_over = true;

        for (auto &c : client->connectedClients)
        {
            if ((c.id != client->painterID && !c.guessedCorrectly) && c.id != client->adminID)
            {
                client->round_over = false;
                break;
            }
        }

        if (client->id == client->adminID && !client->guessed)
        {
            if (client->painterID != client->id)
            {
                client->round_over = false;
            }
        }
    }
}

void Games::EndRound(shared_ptr<Client> &client, string &prevChosenWord, bool &messagesSent)
{
    if (!chosenWord.empty())
    {
        prevChosenWord = chosenWord;
        chosenWord = "";
    }

    if (client->id == client->adminID && !messagesSent)
    {
        UpdatePainterID(client);
        messagesSent = true;
    }

    UpdatePainterStatus(client);

    painter.SetCanPaint(false);
    censoredString = chosenWord;
    DrawTextPro(GetFontDefault(), "0", {55, 95}, {0, 0}, 0, 20, 4, BLACK);

    if (client->round_over)
    {
        ResetRound(client);
    }
}

void Games::UpdatePainterID(shared_ptr<Client> &client)
{
    for (int i = 0; i < client->connectedClients.size(); i++)
    {
        if (client->connectedClients[i].id == client->painterID)
        {
            if (i == client->connectedClients.size() - 1)
            {
                client->painterID = client->connectedClients[0].id;
            }
            else
            {
                client->painterID = client->connectedClients[i + 1].id;
            }

            string msg = "ROUND_OVER";
            client->Send(msg);

            msg = "PAINTER: " + std::to_string(client->painterID);
            cout << "Sending new painter ID: " << msg << endl;
            client->Send(msg);

            break;
        }
    }
}

void Games::UpdatePainterStatus(shared_ptr<Client> &client)
{
    if (client->painterID == client->id)
    {
        SetIsGuesser(false);
    }
    else
    {
        SetIsGuesser(true);
    }
}

void Games::ResetRound(shared_ptr<Client> &client)
{
    for (int i = 0; i < client->connectedClients.size(); i++)
    {
        client->connectedClients[i].guessedCorrectly = false;
    }
    client->guessed = false;

    finished = true;
    client->round_over = false;
}

void Games::DrawWord(shared_ptr<Client> &client, int timeRemaining)
{
    censoredString = CensorWord(chosenWord);
    int i = censoredString.size();
    std::string str = std::to_string(i);
    DrawTextPro(GetFontDefault(), str.c_str(), {(GetScreenWidth() / 2.0f) + (MeasureText(censoredString.c_str(), 20)), 90}, {0, 0}, 0.0f, 10.0f, 2.0f, BLACK);

    if (!isGuesser)
    {
        DrawForPainter(client, timeRemaining);
    }
    else
    {
        DrawForGuesser(client);
    }
}

void Games::DrawForPainter(shared_ptr<Client> &client, int timeRemaining)
{
    if (timeRemaining > 0)
        painter.SetCanPaint(true);
    DrawTextPro(GetFontDefault(), "DIBUJA:", {(GetScreenWidth() / 2.0f) - (MeasureText("Dibuja:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
    DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
    canvas.DrawPalette(palette);
}

void Games::DrawForGuesser(shared_ptr<Client> &client)
{
    if (!guessed)
    {
        painter.SetCanPaint(false);
        vector<string> messages = client->getMessages();

        CheckGuesses(client, messages);

        DrawTextPro(GetFontDefault(), "ADIVINA:", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINA:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
        DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(censoredString.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
    }
    else
    {
        DrawTextPro(GetFontDefault(), "ADIVINA:", {(GetScreenWidth() / 2.0f) - (MeasureText("Adivina:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
        DrawTextPro(GetFontDefault(), chosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(chosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
        DrawTextPro(GetFontDefault(), "ADIVINADO", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINADO", 20) / 2), +300}, {0, 0}, 0, 20, 4, BLACK);
    }
}

void Games::CheckGuesses(shared_ptr<Client> &client, vector<string> &messages)
{
    for (int i = 0; i < messages.size(); i++)
    {
        string message = messages[i];
        regex msgRegex("\\((\\d+)\\)\\s+\\[(\\w+)\\]:\\s+(.*)");
        smatch msgMatch;

        if (regex_search(message, msgMatch, msgRegex) && msgMatch.size() > 3)
        {
            int id = stoi(msgMatch.str(1));
            string name = msgMatch.str(2);
            string msg = msgMatch.str(3);

            string lowerMsg = msg;
            std::transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(), ::tolower);
            string lowerChosenWord = client->chosenWord;
            std::transform(lowerChosenWord.begin(), lowerChosenWord.end(), lowerChosenWord.begin(), ::tolower);

            if (lowerMsg == lowerChosenWord)
            {
                if (id == client->id)
                {
                    int timeRemaining = (drawTime - GetElapsedTime());
                    int points = BASE_POINTS + (timeRemaining) * 10;
                    client->AddPoints(points);
                    client->guessed = true;

                    cout << "Client " << client->id << " guessed the word!" << endl;
                    client->connectedClients[client->id].guessedCorrectly = true;
                }
            }
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

void Games::DrawTimer(int &timer, Texture2D &clock)
{
    if (timer > 0)
    {
        timer--;
    }
    int clockPosX = 20;
    int clockPosY = 60;
    DrawTexture(clock, clockPosX, clockPosY, WHITE);

    // Convert the timer to a string, dividing by FRAMES to convert to seconds
    std::string timerText = std::to_string(timer);

    // Calculate the position to draw the text in the center of the clock
    Vector2 textSize = MeasureTextEx(GetFontDefault(), timerText.c_str(), 20, 4);
    float textPosX = clockPosX + (clock.width / 2) - (textSize.x / 2);
    float textPosY = clockPosY + (clock.height / 2) - (textSize.y / 2);

    // Draw the timer text on the screen centered in the clock
    DrawTextPro(GetFontDefault(), timerText.c_str(), {textPosX, textPosY + 4}, {0, 0}, 0, 20, 4, BLACK);
}

// Filter chat messages to only show the guesses
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

    StartTimer();

    optionWords = GetRandomWords();
}