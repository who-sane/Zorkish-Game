#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <vector>
#include <sstream>
#include "Entity.h"

// util
inline std::string toLowerCase(const std::string &input)
{
    std::string lower = input;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower;
}

// implementation of an adjacency list
class Location
{
public:
    // as per design
    int number;                                                             // location id
    std::string name;                                                       // location name
    std::string description;                                                // description of the location
    std::unordered_map<std::string, std::shared_ptr<Location>> connections; // map of connections by direction

    // init the location with an id, name, and description
    Location(int num, const std::string &nm, const std::string &desc)
        : number(num), name(nm), description(desc) {}

    // method to add a connection in a specific direction
    void addConnection(const std::string &direction, std::shared_ptr<Location> connectedLocation)
    {
        connections[direction] = connectedLocation;
    }

    // add an entity to the location
    void addEntity(std::shared_ptr<Entity> entity)
    {
        entities.push_back(entity);
    }

    // retrieve all entities in the location
    const std::vector<std::shared_ptr<Entity>> &getEntities() const
    {
        return entities;
    }

    // generate a visual description of all top-level entities in the location for LOOK
    std::string getEntityDescriptions() const
    {
        if (entities.empty())
            return "There are no items here.";

        std::ostringstream oss;
        oss << "You notice ";
        bool first = true;
        // iterate over all entities and append
        // their names to the output stream so that 
        // it can be displayed to the player
        for (const auto &entity : entities)
        {
            if (first)
            {
                oss << entity->getName();
                first = false;
            }
            else
            {
                oss << ", " << entity->getName();
            }
        }
        oss << ".\n";
        return oss.str();
    }

    // find an entity by name for LOOK AT
    std::shared_ptr<Entity> findEntityByName(const std::string &name) const
    {
        for (const auto &entity : entities)
        {
            if (toLowerCase(entity->getName()) == toLowerCase(name))
                return entity;
        }
        return nullptr; 
    }

    void removeEntity(std::shared_ptr<Entity> entity)
    {
        auto it = std::find(entities.begin(), entities.end(), entity);
        if (it != entities.end())
        {
            entities.erase(it);
        }
    }

private:
    std::vector<std::shared_ptr<Entity>> entities; // collection of entities within the location
};

#endif
