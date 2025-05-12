#ifndef GRAPH_H
#define GRAPH_H

#include "Location.h"
#include "MessageDispatcher.h"
#include <unordered_map>
#include <string>
#include <memory>

class Graph
{
public:
    // constructor passing dispatcher
    Graph(MessageDispatcher &dispatcher) : dispatcher(dispatcher) {}

    // loads graph data from file
    void loadFromFile(const std::string &filename);

    // displays location details
    void displayLocation(int locationID) const;

    std::unordered_map<int, std::shared_ptr<Location>> locations; // stores locations by id

private:
    MessageDispatcher &dispatcher; // dispatcher reference for message handling
};

#endif
