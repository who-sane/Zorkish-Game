#include "MessageDispatcher.h"
#include <iostream> 

// registers a recipient with a unique id and its message handler
bool MessageDispatcher::registerRecipient(const std::string& id, MessageHandler handler) {
    if (recipients.find(id) != recipients.end()) {
        // err
        std::cerr << "Recipient with ID '" << id << "' is already registered.\n";
        return false;
    }
    recipients[id] = handler;
    return true;
}

// sends a message directly to the recipient
void MessageDispatcher::sendMessage(const Message& message) {
    std::cout << "Sending message from '" << message.from << "' to '" << message.to << "' with message: '" << message.message << "'\n";
    auto it = recipients.find(message.to);
    if (it != recipients.end()) {
        it->second(message); // call the recipient's handler
    } else {
        // err
        std::cerr << "No recipient found for ID '" << message.to << "'.\n";
    }
}
