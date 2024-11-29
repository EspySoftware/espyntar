#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <regex>
#include <iostream>

using std::string;
using std::cout;
using std::endl;
using std::regex;
using std::smatch;
using std::stoi;
using std::regex_search;

// ADMIN command
// Format: "ADMIN: 1"
class AdminMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        regex r("ADMIN:\\s+(\\d+)");
        smatch match;
        if (regex_search(message, match, r) && match.size() > 1) {
            int adminID = stoi(match.str(1));
            client.adminID = adminID;
            cout << "Admin set to ID: " << adminID << endl;
        }
    }
};