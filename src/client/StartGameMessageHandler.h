#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <iostream>

using std::cout;
using std::endl;
using std::string;

// START_GAME command
// Format: "START_GAME"
class StartGameMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        if (message.find("START_GAME") == 0) {
            client.messages.push_back("The game has started.");
            cout << "Game started." << endl;
        }
    }
};