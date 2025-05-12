#include "Graph.h"
#include "Location.h"
#include "./FunctionalComponents/TakeableComponent.h"
#include "./FunctionalComponents/ContainerComponent.h"
#include "./FunctionalComponents/OpenableComponent.h"
#include "./FunctionalComponents/UseableComponent.h"
#include "./AttributeComponents/LockableComponent.h"
#include "./AttributeComponents/HealthComponent.h"
#include "MessageDispatcher.h" // include dispatcher

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

// we love to trim whitespace from a string
static std::string trim(const std::string &str)
{
    // find the first and last non-whitespace characters
    // thanks to https://stackoverflow.com/questions/25829143/trim-whitespace-from-a-string
    // https://cplusplus.com/reference/string/string/find_first_not_of/
    size_t start = str.find_first_not_of(" \t\n\r");
    // https://cplusplus.com/reference/string/string/find_last_not_of/
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// helper function to count leading spaces or tabs for indentation level
int countIndentation(const std::string &line)
{
    int count = 0;
    for (char c : line)
    {
        if (c == ' ' || c == '\t')
            ++count;
        else
            break;
    }
    return count;
}

// load location and connection data from the single-line world file
// rewrote to handle nested entities
void Graph::loadFromFile(const std::string &filename)
{
    std::cout << "Opening file: " << filename << std::endl;
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    std::cout << "File opened successfully." << std::endl;

    std::cout << "loading world from file: " << filename << std::endl;

    std::string line;
    std::shared_ptr<Location> currentLocation = nullptr;
    std::shared_ptr<Entity> currentContainer = nullptr;
    int containerIndentationLevel = -1;
    
    // stores connections to be processed later (hierarchy)
    std::vector<std::tuple<int, std::string, int>> pendingConnections;

    while (std::getline(file, line))
    {
        int currentIndentationLevel = countIndentation(line);
        std::string trimmedLine = trim(line);

        if (trimmedLine.empty())
        {
            std::cout << "skipping blank line\n";
            continue; 
        }

        try
        {
            std::cout << "processing line: " << trimmedLine << std::endl;

            // handles location parsing
            if (isdigit(trimmedLine[0]))
            {
                std::stringstream ss(trimmedLine);
                std::string idStr, name, description, connectionsStr;

                std::getline(ss, idStr, ';');
                int locID = std::stoi(trim(idStr));
                std::getline(ss, name, ';');
                name = trim(name);
                std::getline(ss, description, ';');
                description = trim(description);

                std::cout << "creating location: id=" << locID << ", name=" << name
                          << ", description=" << description << std::endl;

                currentLocation = std::make_shared<Location>(locID, name, description);
                locations[locID] = currentLocation;

                // stores connections for later processing
                std::getline(ss, connectionsStr, ';');
                std::stringstream connectionsStream(connectionsStr);
                std::string connection;

                while (std::getline(connectionsStream, connection, ','))
                {
                    std::stringstream connectionStream(connection);
                    std::string direction, connectedLocIDStr;

                    std::getline(connectionStream, direction, '=');
                    std::getline(connectionStream, connectedLocIDStr);

                    direction = trim(direction);
                    int connectedLocID = std::stoi(trim(connectedLocIDStr));
                    pendingConnections.emplace_back(locID, direction, connectedLocID);
                }

                // registres location with dispatcher using unique ID
                std::string locId = "location_" + std::to_string(locID);
                dispatcher.registerRecipient(locId, [currentLocation, this](const Message &msg) {
                    // handnles location-specific messages here
                    if (msg.message == "removeItem") {
                        std::string itemName = std::any_cast<std::string>(msg.data);
                        auto entity = currentLocation->findEntityByName(itemName);
                        if (entity && entity->getComponent<TakeableComponent>()) {
                            currentLocation->removeEntity(entity);
                            // Send the item to the player
                            dispatcher.sendMessage({"location", "player", "addItem", entity});
                        } else {
                            std::cout << "You can't take the " << itemName << ".\n";
                        }
                    }
                });

                currentContainer = nullptr;
                containerIndentationLevel = -1;
            }
            // handlse entity parsing
            else if (currentLocation)
            {
                std::stringstream ss(trimmedLine);
                std::string entityName, entityDescription, propertiesStr;

                std::getline(ss, entityName, ':');
                entityName = trim(entityName);
                std::getline(ss, entityDescription, ';');
                entityDescription = trim(entityDescription);

                if (entityName.empty() || entityDescription.empty())
                {
                    std::cerr << "warning: skipped entity due to missing name or description: " << trimmedLine << std::endl;
                    continue; // skip invalid entities
                }

                std::cout << "creating entity: name=" << entityName
                          << ", description=" << entityDescription << std::endl;

                auto entity = std::make_shared<Entity>(entityName, entityDescription, dispatcher);
                bool isContainer = false;

                if (std::getline(ss, propertiesStr))
                {
                    propertiesStr = trim(propertiesStr);
                    std::cout << "parsing properties for entity: " << propertiesStr << std::endl;

                    if (propertiesStr.find("Takeable") != std::string::npos)
                    {
                        entity->addComponent(std::make_shared<TakeableComponent>());
                        std::cout << "added component: TakeableComponent\n";
                    }
                    if (propertiesStr.find("Container") != std::string::npos)
                    {
                        entity->addComponent(std::make_shared<ContainerComponent>());
                        isContainer = true;
                        std::cout << "added component: ContainerComponent\n";
                    }
                    if (propertiesStr.find("Openable") != std::string::npos)
                    {
                        entity->addComponent(std::make_shared<OpenableComponent>());
                        std::cout << "added component: OpenableComponent\n";
                    }
                    if (propertiesStr.find("Lockable=") != std::string::npos)
                    {
                        std::regex lockRegex(R"(Lockable=([^,\]]+))");
                        std::smatch match;
                        if (std::regex_search(propertiesStr, match, lockRegex))
                        {
                            entity->addComponent(std::make_shared<LockableComponent>(match[1]));
                            std::cout << "added component: LockableComponent with key=" << match[1] << "\n";
                        }
                    }
                    if (propertiesStr.find("Health=") != std::string::npos)
                    {
                        std::regex healthRegex(R"(Health=\+?(\d+))");
                        std::smatch match;
                        if (std::regex_search(propertiesStr, match, healthRegex))
                        {
                            entity->addComponent(std::make_shared<HealthComponent>(std::stoi(match[1])));
                            std::cout << "added component: HealthComponent with health=" << match[1] << "\n";
                        }
                    }
                    if (propertiesStr.find("Usable") != std::string::npos)
                    {
                        if (propertiesStr.find("Health=") != std::string::npos)
                        {
                            size_t pos = propertiesStr.find("Health=");
                            int healthEffect = std::stoi(trim(propertiesStr.substr(pos + 7)));
                            entity->addComponent(std::make_shared<UsableComponent>(
                                healthEffect > 0 ? UseEffectType::HEAL : UseEffectType::DAMAGE, healthEffect));
                            std::cout << "added component: UsableComponent with healthEffect=" << healthEffect << "\n";
                        }
                        else
                        {
                            entity->addComponent(std::make_shared<UsableComponent>());
                            std::cout << "added component: UsableComponent\n";
                        }
                    }
                }

                dispatcher.registerRecipient(entity->getName(), [entity](const Message &msg)
                                             { entity->handleMessage(msg); });
                std::cout << "registered entity: " << entity->getName() << " with dispatcher\n";

                if (currentIndentationLevel > containerIndentationLevel && currentContainer && currentContainer->getComponent<ContainerComponent>())
                {
                    currentContainer->getComponent<ContainerComponent>()->addItem(entity);
                    std::cout << "added entity: " << entity->getName() << " to container: " << currentContainer->getName() << "\n";
                }
                else
                {
                    currentLocation->addEntity(entity);
                    std::cout << "added entity: " << entity->getName() << " to location: " << currentLocation->name << "\n";

                    if (isContainer)
                    {
                        currentContainer = entity;
                        containerIndentationLevel = currentIndentationLevel;
                    }
                    else
                    {
                        currentContainer = nullptr;
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "error processing line: " << trimmedLine << " - " << e.what() << std::endl;
        }
    }

    // process all connections after all locations are loaded
    for (const auto &[fromID, direction, toID] : pendingConnections)
    {
        if (locations.find(fromID) != locations.end() && locations.find(toID) != locations.end())
        {
            locations[fromID]->addConnection(direction, locations[toID]);
            std::cout << "Added connection from location " << fromID << " to location " 
                     << toID << " in direction " << direction << std::endl;
        }
    }

    std::cout << "finished loading world from file: " << filename << std::endl;
}