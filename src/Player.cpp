#include "Player.h"
#include <iostream>
#include <cstdlib>

// constructor implementation (must match declaration in Player.h)
Player::Player(int startLocation, Graph &gameGraph, MessageDispatcher &dispatcher)
    : currentLocation(startLocation), graph(gameGraph), dispatcher(dispatcher)
{
    dispatcher.registerRecipient("player", [this](const Message &msg)
                                 { handleMessage(msg); });
}

void Player::displayCurrentLocation() const
{
    const auto &location = graph.locations.at(currentLocation);

    std::cout << "\n"
              << location->name << "\n"
              << location->description << "\n";

    std::string entityDescriptions = location->getEntityDescriptions();
    if (!entityDescriptions.empty())
    {
        std::cout << entityDescriptions;
    }

    std::cout << "\n";
}

void Player::go(const std::string &direction)
{
    if (graph.locations[currentLocation]->connections.count(direction))
    {
        currentLocation = graph.locations[currentLocation]->connections[direction]->number;
        std::cout << "\nYou move " << direction << ".\n";
    }
    else
    {
        std::cout << "\nNo path in that direction.\n";
    }
}

void Player::addItemToInventory(std::shared_ptr<Entity> item)
{
    inventory.push_back(item);
}

void Player::viewInventory() const
{
    std::cout << "\n----- Inventory -----\n";
    if (inventory.empty())
    {
        std::cout << "Your inventory is empty.\n";
    }
    else
    {
        for (const auto &item : inventory)
        {
            // display only the name of each item without nested contents
            std::cout << "- " << item->getName() << "\n";
        }
    }
    std::cout << "---------------------\n";
}

std::shared_ptr<Entity> Player::findEntityInInventory(const std::string &name) const
{
    std::string lowerName = toLowerCase(name); // convert input name to lowercase
    for (const auto &item : inventory)
    {
        if (toLowerCase(item->getName()) == lowerName)
        {
            return item;
        }
    }
    return nullptr;
}

int Player::getCurrentLocation() const
{
    return currentLocation;
}

void Player::removeItemFromInventory(std::shared_ptr<Entity> item)
{
    auto it = std::find(inventory.begin(), inventory.end(), item);
    if (it != inventory.end())
    {
        inventory.erase(it);
    }
}

void Player::modifyHealth(int amount)
{
    if (amount < 0)
    {
        takeDamage(-amount);
    }
    else
    {
        health += amount;
        if (health > 5)
        {
            health = 5;
        }
        std::cout << "Your health is now: " << health << "\n";
    }
}

void Player::takeDamage(int amount)
{
    health -= amount; // reduce health by amount
    if (health <= 0)
    {
        health = 0; // caps lowest hp at 0
        std::cout << "You took " << amount << " damage. Your health is now: " << health << "\n";
        std::cout << "You lose! Game over.\n";
        std::cout << "Press enter to exit...\n";
        std::cin.get();
        exit(0); // quit
    }
    else
    {
        std::cout << "You took " << amount << " damage. Your health is now: " << health << "\n";
    }
}

void Player::handleMessage(const Message &msg)
{
    if (msg.message == "heal")
    {
        modifyHealth(std::any_cast<int>(msg.data));
    }
    else if (msg.message == "damage")
    {
        takeDamage(std::abs(std::any_cast<int>(msg.data)));
    }
    else if (msg.message == "addItem")
    {
        auto item = std::any_cast<std::shared_ptr<Entity>>(msg.data);
        addItemToInventory(item);
        std::cout << "You received " << item->getName() << ".\n";
    }
    else if (msg.message == "removeItem")
    {
        auto item = std::any_cast<std::shared_ptr<Entity>>(msg.data);
        removeItemFromInventory(item);
        std::cout << "You lost " << item->getName() << ".\n";
    }
}
