#pragma once
#include <string>
#include <any>

// represents a single message in the messaging system
struct Message {
    std::string from;  // sender's unique id
    std::string to;    // recipient's unique id
    std::string message; // action or type of message
    std::any data;     // optional payload for additional information
};
