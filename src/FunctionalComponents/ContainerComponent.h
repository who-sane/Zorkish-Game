#pragma once
#include "../Component.h"
#include <vector>
#include <memory>

class Entity;  // crcluar depednecies

class ContainerComponent : public Component {
public:
    // adds an item to the container
    void addItem(std::shared_ptr<Entity> item) {
        contents.push_back(item);
    }

    // removes an item from the container
    void removeItem(std::shared_ptr<Entity> item) {
        contents.erase(std::remove(contents.begin(), contents.end(), item), contents.end());
    }

    // retrieves the container's contents
    const std::vector<std::shared_ptr<Entity>>& getContents() const {
        return contents;
    }

    // checks if the container has items
    bool isEmpty() const {
        return contents.empty();
    }

private:
    std::vector<std::shared_ptr<Entity>> contents; // stores items in the container
};
