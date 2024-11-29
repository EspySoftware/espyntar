#pragma once
#include "./MessageHandler.h"
#include "../headers/Client.h"
#include <regex>
#include <iostream>

using std::regex;
using std::smatch;
using std::stoi;
using std::regex_search;
using std::vector;
using std::string;

// PAINT COMMAND
// Format: "PAINT:100,200,1,10" (x, y, color, brushSize)
class PaintMessageHandler : public MessageHandler {
public:
    void handle(Client& client, const string& message) override {
        regex r("PAINT:(\\d+),(\\d+),(\\d+),(\\d+)");
        smatch match;
        if (regex_search(message, match, r) && match.size() > 4) {
            int x = stoi(match.str(1));
            int y = stoi(match.str(2));
            int color = stoi(match.str(3));
            int brushSize = stoi(match.str(4));

            if (!client.paintMessages.empty()) {
                PaintMessage lastMessage = client.paintMessages.back();
                vector<Point> interpolatedPoints = client.interpolatePoints(
                    {lastMessage.x, lastMessage.y}, {x, y}, brushSize);

                for (const auto& point : interpolatedPoints) {
                    client.paintMessages.push_back(PaintMessage{point.x, point.y, color, static_cast<float>(brushSize)});
                }
            }

            client.paintMessages.push_back(PaintMessage{ x, y, color, static_cast<float>(brushSize) });
        }
    }
};