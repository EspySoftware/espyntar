#include <fstream>
#include <iostream>
#include <unordered_set>
#include "../headers/Games.h"
#include "../headers/raygui.h"

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

void Games::SetChosenWord(shared_ptr<Client> &client, Texture2D &clock)
{
    stringstream msg;
    client->guessed = false;

    int timeRemaining = (setTime - GetElapsedTime());
    DrawTimer(timeRemaining, clock);

    // Parse messages for "Word has been chosen." message (regular client)
    vector<string> messages = client->getMessages();
    for (int i = 0; i < messages.size(); i++)
    {
        if (messages[i] == "Word has been chosen.")
        {
            // Remove message from messages
            client->messages.erase(client->messages.begin() + i);

            // Set the chosen word
            chosenWord = client->chosenWord;
            censoredString = CensorWord(chosenWord);
            chosen = true;

            break;
        }
    }

    if (isGuesser && GetElapsedTime() >= setTime && !chosen)
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
        // Verify that the client is the painter
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
    }
    else
    {
        // Check if the client is the painter
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

void Games::DrawChosenWord(shared_ptr<Client> &client, Texture2D &clock)
{
    string prevChosenWord;
    int timeRemaining = (drawTime - GetElapsedTime());
    static bool messagesSent = false;

    // If admin, check if all clients have guessed the word
    if (client->id == client->adminID)
    {
        client->round_over = true;

        // Check if other clients have guessed the word
        for (auto &c : client->connectedClients)
        {
            if ((c.id != client->painterID && !c.guessedCorrectly) && c.id != client->adminID)
            {
                client->round_over = false;
                break;
            }
        }

        // Check if admin has guessed the word
        if (client->id == client->adminID && !client->guessed) {
            // If the client isnt PAINTER and hasnt guessed the word, set round_over to false
            if (client->painterID != client->id)
            {
                client->round_over = false;
            }
        }
        
    }

    // If the timer is up, end the round
    if (GetElapsedTime() >= drawTime)
    {
        client->round_over = true;
    }

    // If the timer is up or all clients have guessed the word, end the round
    if (client->round_over)
    {
        // Reset the chosen word
        if (!chosenWord.empty())
        {
            prevChosenWord = chosenWord;
            chosenWord = "";
        }

        // If admin, update painter ID to the next client
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
        censoredString = chosenWord;
        DrawTextPro(GetFontDefault(), "0", {55, 95}, {0, 0}, 0, 20, 4, BLACK);

        // If the timer is up, end the round
        if (client->round_over)
        {
            // Reset the guessedCorrectly flag for all clients
            for (int i = 0; i < client->connectedClients.size(); i++)
            {
                client->connectedClients[i].guessedCorrectly = false;
            }
            client->guessed = false;

            finished = true; // Ends round
            messagesSent = false;
            client->round_over = false; 
            return;
        }
    }
    else
    {
        timeRemaining = (drawTime - GetElapsedTime());
        DrawTimer(timeRemaining, clock);
    }

    // Draw the number of letters in the word
    censoredString = CensorWord(chosenWord);
    int i = censoredString.size();
    std::string str = std::to_string(i);
    DrawTextPro(GetFontDefault(), str.c_str(), {(GetScreenWidth() / 2.0f) + (MeasureText(censoredString.c_str(), 20)), 90}, {0, 0}, 0.0f, 10.0f, 2.0f, BLACK);

    // Draw the word
    if (!isGuesser)
    {
        int timeRemaining = (setTime - GetElapsedTime());
        if (timeRemaining > 0)
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
            // vector<string> filtered = FilterChat(messages);

            // Check if the client guessed the word
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
                            int points = BASE_POINTS + (timeRemaining) * 10; // Points based on time remaining
                            client->AddPoints(points);
                            client->guessed = true;

                            cout << "Client " << client->id << " guessed the word!" << endl;
                            client->connectedClients[client->id].guessedCorrectly = true;
                        }
                    }
                }
            }

            // Draw the word
            DrawTextPro(GetFontDefault(), "ADIVINA:", {(GetScreenWidth() / 2.0f) - (MeasureText("ADIVINA:", 25) / 2), 60.0f}, {0, 0}, 0.0f, 25, 3.0f, BLACK);
            DrawTextPro(GetFontDefault(), censoredString.c_str(), {(GetScreenWidth() / 2.0f) - (MeasureText(censoredString.c_str(), 20) / 2), +100}, {0, 0}, 0, 20, 4, BLACK);
            
            // timeRemaining = (drawTime - GetElapsedTime());

            // // Reveal letters at certain times
            // std::unordered_set<int> revealedIndices;

            // // Reveal letters in the word
            // if (timeRemaining == 15 || timeRemaining == 30 || timeRemaining == 60)
            // {
            //     int randIndex;

            //     do {
            //         randIndex = rand() % static_cast<int>(chosenWord.size());
            //     } while (revealedIndices.find(randIndex) != revealedIndices.end());

            //     censoredString.at(randIndex) = chosenWord.at(randIndex);
            //     revealedIndices.insert(randIndex);
            // }
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