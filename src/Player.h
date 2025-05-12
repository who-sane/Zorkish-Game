#ifndef PLAYER_H
#define PLAYER_H

#include "Graph.h"
#include "Entity.h"
#include "MessageDispatcher.h"
#include <vector>
#include <string>

// this class is responsible for managing the player's location and movement as per spike report design
class Player
{
public:
    // constructor declaration
    Player(int startingLocation, Graph &graph, MessageDispatcher &dispatcher);

    void displayCurrentLocation() const;
    void go(const std::string &direction);
    void viewInventory() const;
    int getCurrentLocation() const;
    void addItemToInventory(std::shared_ptr<Entity> item);
    std::shared_ptr<Entity> findEntityInInventory(const std::string &name) const;
    void removeItemFromInventory(std::shared_ptr<Entity> item);
    void modifyHealth(int amount);
    void takeDamage(int amount);
    void handleMessage(const Message &msg);

private:
    int currentLocation;                            // id of the current location
    Graph &graph;                                   // reference to the game graph
    std::vector<std::shared_ptr<Entity>> inventory; // inventory storing Entity pointers
    int health = 5;                                 // player's health
    MessageDispatcher &dispatcher;                 // reference to the shared message dispatcher
};

#endif
