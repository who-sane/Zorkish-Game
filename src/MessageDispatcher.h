#pragma once
#include "Message.h"
#include <unordered_map>
#include <functional>

// central hub for sending and receiving messages
class MessageDispatcher {
public:
    using MessageHandler = std::function<void(const Message&)>;

    // registers a recipient with a unique id and its message handler
    bool registerRecipient(const std::string& id, MessageHandler handler);

    // sends a message directly to the recipient
    void sendMessage(const Message& message);

private:
    std::unordered_map<std::string, MessageHandler> recipients; // registered recipients
};
