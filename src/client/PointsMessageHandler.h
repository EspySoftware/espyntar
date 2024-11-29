#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <regex>
#include <iostream>

using std::regex;
using std::smatch;
using std::stoi;
using std::string;
using std::cout;
using std::endl;

// POINTS command
// Format: "(id) points: 100."
class PointsMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        regex r("\\((\\d+)\\)\\s+points:\\s+(\\d+)\\.");
        smatch match;
        if (regex_search(message, match, r) && match.size() > 2) {
            int id = stoi(match.str(1));
            int points = stoi(match.str(2));
            cout << "Server: Giving client (" << id << ") " << client.name << " " << points << " points." << endl;

            // Update the points of the client
            for (auto& otherClient : client.connectedClients) {
                if (otherClient.id == id) {
                    otherClient.points += points;
                    otherClient.guessedCorrectly = true;
                    break;
                }
            }
        } else {
            cout << "Invalid points message format: " << message << endl;
        }
    }
};