#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <iostream>

using std::string;
using std::cout;
using std::endl;

// ROUND_OVER command
// Format: "ROUND_OVER"
class RoundOverMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        if (message.find("ROUND_OVER") == 0) {
            client.round_over = true;
            client.chosenWord = "";
            client.messages.push_back("The round is over.");
            cout << "Round over." << endl;
        }
    }
};