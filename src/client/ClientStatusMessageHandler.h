#pragma once
#include "MessageHandler.h"
#include "../headers/Client.h"
#include <regex>
#include <iostream>
#include <algorithm>

using std::regex;
using std::smatch;
using std::stoi;
using std::string;
using std::cout;
using std::endl;
using std::remove_if;

// Add and remove clients from the vector
// Format: (1) Client1 disconnected. or (22) Client22 connected.
class ClientStatusMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        regex r(R"(\((\d+)\)\s+(\w+)\s+(connected|disconnected)\.)");
        smatch match;
        if (regex_search(message, match, r) && match.size() > 3) {
            int id = stoi(match.str(1));
            string name = match.str(2);
            string status = match.str(3);

            if (status == "connected") {
                client.connectedClients.push_back({id, name, 0});
                cout << "Added client: " << name << " with ID: " << id << endl;
            }
            else if (status == "disconnected") {
                auto it = remove_if(client.connectedClients.begin(), client.connectedClients.end(),
                    [&](const OtherClient& c) { return c.id == id; });
                if (it != client.connectedClients.end()) {
                    client.connectedClients.erase(it, client.connectedClients.end());
                    cout << "Removed client with ID: " << id << endl;
                }
            }
        } else {
            cout << "Invalid client status message format: " << message << endl;
        }
    }
};