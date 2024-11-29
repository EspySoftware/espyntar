#pragma once
#include <string>
class Client;

class MessageHandler {
public:
    virtual ~MessageHandler() = default;
    virtual void handle(Client& client, const std::string& message) = 0;
};