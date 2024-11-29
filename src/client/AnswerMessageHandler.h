#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <string>
#include <iostream>

using std::string;
using std::cout;
using std::endl;

// ANSWER command
// Format: "ANSWER: WORD"
class AnswerMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        string word = message.substr(8);
        cout << "The word is: " << word << endl;
        client.chosenWord = word;
        client.messages.push_back("Word has been chosen.");
    }
};