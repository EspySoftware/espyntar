#include <fstream>
#include <iostream>
#include "../headers/Games.h"
#include "../headers/raygui.h"

#define FRAMES 144
#define BASE_POINTS 200

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
    client->guessed = false;

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
            client->chosenWord = chosenWord;
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
            client->chosenWord = chosenWord;
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
            client->chosenWord = chosenWord;
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
            client->chosenWord = chosenWord;
            censoredString = CensorWord(chosenWord);

            // Broadcast chosen word
            cout << chosenWord << endl;
            msg << "ANSWER: " << chosenWord;
            client->Send(msg.str());
            chosen = true;
            painter.SetColor(22);
        }

        prevChosenWord = chosenWord;
    }
    else
    {
        // Look for painter name
        string painterName;
        for (int i = 0; i < client->connectedClients.size(); i++)
        {
            if (client->connectedClients[i].id == client->painterID)
            {
                painterName = client->connectedClients[i].name;
                break;
            }
        }

        string esperando = "Esperando a " + painterName;

        DrawRectangle(GetScreenWidth() / 2 - 700 / 2, GetScreenHeight() / 2 - 560 / 2.0f + 70.0f, 700, 560, {102, 149, 89, 200}); // cuadro transparente
        DrawTextPro(GetFontDefault(), esperando.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(esperando.c_str(), 20) / 2), GetScreenHeight() - 500.0f}, {0, 0}, 0, 20, 4, BLACK);
    }
}

void Games::DrawChosenWord(shared_ptr<Client> &client)
{
    static bool messagesSent = false;

    if (drawTimer < 144)
    {
        if (!chosenWord.empty())
        {
            prevChosenWord = chosenWord;
            chosenWord = "";
        }

        // If admin, update painter ID to the next client (if last client, set painter to the first client)
        if (client->id == client->adminID && !messagesSent)
        {
            for (int i = 0; i < client->connectedClients.size(); i++)
            {
                // Find the painter ID and update it
                if (client->connectedClients[i].id == client->painterID)
                {
                    // If last client, set painter to the first client
                    if (i == client->connectedClients.size() - 1)
                    {
                        client->painterID = client->connectedClients[0].id;
                    }
                    // Else, set painter to the next client
                    else
                    {
                        client->painterID = client->connectedClients[i + 1].id;
                    }

                    // Send ROUND_OVER message
                    string msg = "ROUND_OVER";
                    client->Send(msg);

                    // Send new painter ID
                    msg = "PAINTER: " + std::to_string(client->painterID);
                    cout << "Sending new painter ID: " << msg << endl;
                    client->Send(msg);

                    messagesSent = true;
                    break;
                }
            }
        }

        // Update if client is painter
        if (client->painterID == client->id)
        {
            SetIsGuesser(false);
        }
        else
        {
            SetIsGuesser(true);
        }

        painter.SetCanPaint(false);
        censoredString = prevChosenWord;
        DrawTextPro(GetFontDefault(), "0", {55, 95}, {0, 0}, 0, 20, 4, BLACK);
        drawTimer--;
        if (drawTimer < -(5 * FRAMES))
        {
            finished = true; // Ends round
            messagesSent = false;
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
        DrawTextPro(GetFontDefault(), prevChosenWord.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(prevChosenWord.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
        canvas.DrawPalette(palette);
    }
    else
    {
        if (!guessed)
        {
            // cout << chosenWord << endl;
            painter.SetCanPaint(false);
            vector<string> messages = client->getMessages();
            // vector<string> filtered = FilterChat(messages);

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

                    // Lowercased message to lowercased chosenWord
                    string lowerMsg = msg;
                    std::transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(), ::tolower);
                    string lowerChosenWord = client->chosenWord;
                    std::transform(lowerChosenWord.begin(), lowerChosenWord.end(), lowerChosenWord.begin(), ::tolower);

                    if (lowerMsg == lowerChosenWord)
                    {
                        if (id == client->id)
                        {
                            client->AddPoints(BASE_POINTS + (drawTimer / 144));
                            client->guessed = true;
                            guessed = true;
                        }
                    }
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
    drawTimer = 10 * FRAMES;
    setTimer = 10 * FRAMES;
    optionWords = GetRandomWords();
}
