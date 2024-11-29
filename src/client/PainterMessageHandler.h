#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <regex>
#include <iostream>

using std::regex;
using std::smatch;
using std::stoi;
using std::string;
using std::regex_search;
using std::cout;
using std::endl;

// PAINTER command
// Format: "PAINTER: 1"
class PainterMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        regex r("PAINTER:\\s+(\\d+)");
        smatch match;
        if (regex_search(message, match, r) && match.size() > 1) {
            int painterID = stoi(match.str(1));
            client.painterID = painterID;
            cout << "Painter set to ID: " << painterID << endl;
        }
    }
};