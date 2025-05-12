#include "Command.h"
#include "Game.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include "MessageDispatcher.h"

// trims whitespace from both ends of a string
static std::string trim(const std::string &str)
{
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// look in command - display contents of a container
void LookInCommand::execute(Game &game, const std::string &args)
{
    std::string entityName = toLowerCase(trim(args));

    // std::cout << "Executing LookInCommand with argument: " << args << "\n";

    // get current location from the player
    int locationID = game.player.getCurrentLocation();
    const auto &location = game.graph.locations[locationID];

    // search for the entity by name
    auto entity = location->findEntityByName(entityName);
    if (!entity)
    {
        // std::cout << "Container not in location, checking inventory...\n";
        entity = game.player.findEntityInInventory(entityName);
    }

    if (!entity)
    {
        std::cout << "You don't see a " << entityName << " here.\n";
        return;
    }

    // send a "look_in" message to the entity
    game.dispatcher.sendMessage({"player", entity->getName(), "look_in"});
}

// go command - change player location
void GoCommand::execute(Game &game, const std::string &args)
{
    if (!args.empty())
    {
        std::string direction = toLowerCase(trim(args));
        game.player.go(direction);
    }
    else
    {
        std::cout << "Specify a direction to move.\n";
    }
}

// help command - display available commands
void HelpCommand::execute(Game &game, const std::string &args)
{
    std::cout << "\nAvailable commands:\n";
    std::cout << "\nDisclaimer: A 'container' is an item that can have other items inside!:\n";

    // navigation
    std::cout << "\n--- Navigation Commands ---\n";
    std::cout << "GO [Compass direction]\n";

    // inspection
    std::cout << "\n--- Inspection Commands ---\n";
    std::cout << "LOOK\n";
    std::cout << "LOOK AT [entity]\n";
    std::cout << "LOOK IN [container]\n";

    // inventory
    std::cout << "\n--- Inventory Commands ---\n";
    std::cout << "INVENTORY\n";
    std::cout << "TAKE [item] FROM [container]\n";
    std::cout << "PUT [item] IN [container]\n";
    std::cout << "OPEN [locked container] WITH [item]\n";
    std::cout << "USE [item] \n";
    // system
    std::cout << "\n--- System Commands ---\n";
    std::cout << "HELP\n";
    std::cout << "ALIAS [new command] [existing command]\n";
    std::cout << "DEBUG\n";
    std::cout << "QUIT\n";
}

// inventory command - display player's inventory
void InventoryCommand::execute(Game &game, const std::string &args)
{
    game.player.viewInventory();
}

// look command - inspect entities or surroundings
void LookCommand::execute(Game &game, const std::string &args)
{
    std::string cleanArgs = trim(args);

    if (cleanArgs.empty())
    {
        game.player.displayCurrentLocation();
        return;
    }

    if (cleanArgs.rfind("at ", 0) == 0)
    {
        std::string entityName = trim(cleanArgs.substr(3));

        std::shared_ptr<Entity> entity = game.graph.locations[game.player.getCurrentLocation()]->findEntityByName(entityName);
        if (!entity)
        {
            entity = game.player.findEntityInInventory(entityName);
        }

        if (entity)
        {
            // send an "inspect" message to the entity
            game.dispatcher.sendMessage({"player", entity->getName(), "inspect"});
        }
        else
        {
            std::cout << "You don't see a " << entityName << " here.\n";
        }
    }
    else
    {
        std::cout << "Invalid look command. Use 'look' for the location, 'look at [entity]', or 'look in [container]'.\n";
    }
}

// alias command - creates new command keywords
void AliasCommand::execute(Game &game, const std::string &args)
{
    std::istringstream iss(args);
    std::string newCommand, existingCommand;
    iss >> newCommand >> existingCommand;

    if (!newCommand.empty() && !existingCommand.empty())
    {
        game.commandManager.addAlias(newCommand, existingCommand);
        std::cout << "Alias created: '" << newCommand << "' for '" << existingCommand << "'.\n";
    }
    else
    {
        std::cout << "Usage: ALIAS [new command] [existing command]\n";
    }
}

// debug tree command - displays game graph
void DebugTreeCommand::execute(Game &game, const std::string &args)
{
    std::cout << "\n--- Game World Debug Tree ---\n";

    for (const auto &[locationID, loc] : game.graph.locations)
    {
        std::cout << "\nLocation ID: " << locationID << "\n";
        std::cout << "Name: " << loc->name << "\n";
        std::cout << "Description: " << loc->description << "\n";

        if (game.player.getCurrentLocation() == locationID)
        {
            std::cout << " -----------------\n | You are here | \n ----------------- \n";
        }

        std::cout << "Connections:\n";
        for (const auto &[direction, connectedLoc] : loc->connections)
        {
            if (connectedLoc)
            {
                std::cout << " - " << direction << " (to location " << connectedLoc->number << " - " << connectedLoc->name << ")\n";
            }
            else
            {
                std::cout << " - " << direction << " (to an Unknown Location)\n";
            }
        }

        std::cout << "Entities:\n";
        for (const auto &entity : loc->getEntities())
        {
            std::cout << " - " << entity->getName() << ": " << entity->getDescription() << "\n";
            if (auto container = entity->getComponent<ContainerComponent>())
            {
                const auto &contents = container->getContents();
                if (!contents.empty())
                {
                    std::cout << "   Contains:\n";
                    for (const auto &item : contents)
                    {
                        std::cout << "   - " << item->getName() << ": " << item->getDescription() << "\n";
                    }
                }
            }
        }

        std::cout << "--------------------------\n";
    }

    std::cout << "\n--- End of Debug Tree ---\n";
}

// quit command - exits the game
void QuitCommand::execute(Game &game, const std::string &args)
{
    std::cout << "Quitting the game...\n";
    exit(0);
}

// take command - picks up an item from location or container
void TakeCommand::execute(Game &game, const std::string &args)
{
    std::istringstream iss(args);
    std::string itemName, containerName;
    iss >> itemName;
    itemName = toLowerCase(trim(itemName));

    std::string keyword;
    if (iss >> keyword && keyword == "from")
    {
        iss >> containerName;
        containerName = toLowerCase(trim(containerName));
    }

    if (!itemName.empty())
    {
        if (!containerName.empty())
        {
            // Find container and send take_from message
            auto container = game.graph.locations[game.player.getCurrentLocation()]->findEntityByName(containerName);
            if (!container)
            {
                container = game.player.findEntityInInventory(containerName);
            }
            if (container)
            {
                game.dispatcher.sendMessage({"player", container->getId(), "take_from", itemName});
            }
            else
            {
                std::cout << "You don't see " << containerName << " here.\n";
            }
        }
        else
        {
            std::string locId = "location_" + std::to_string(game.player.getCurrentLocation());
            game.dispatcher.sendMessage({"player", locId, "removeItem", itemName});
        }
    }
    else
    {
        std::cout << "What do you want to take?\n";
    }
}

// put command - places an item into a container
void PutCommand::execute(Game &game, const std::string &args)
{
    std::istringstream iss(args);
    std::string itemName, containerName;
    iss >> itemName;
    itemName = toLowerCase(trim(itemName));

    std::string keyword;
    if (iss >> keyword && keyword == "in")
    {
        iss >> containerName;
        containerName = toLowerCase(trim(containerName));
    }
    else
    {
        std::cout << "Usage: PUT [item] IN [container]\n";
        return;
    }

    auto item = game.player.findEntityInInventory(itemName);
    if (!item)
    {
        std::cout << "You don't have a " << itemName << " to put anywhere.\n";
        return;
    }

    auto container = game.graph.locations[game.player.getCurrentLocation()]->findEntityByName(containerName);
    if (!container)
    {
        container = game.player.findEntityInInventory(containerName);
    }

    if (!container)
    {
        std::cout << "You don't see a " << containerName << " here.\n";
        return;
    }

    // let the container handle all checks and actions via message
    game.dispatcher.sendMessage({"player", container->getId(), "put_item", item});
}

void OpenCommand::execute(Game &game, const std::string &args)
{
    std::istringstream iss(args);
    std::string containerName, withKeyword, keyName;
    iss >> containerName >> withKeyword >> keyName;

    containerName = toLowerCase(trim(containerName));
    keyName = toLowerCase(trim(keyName));

    if (withKeyword != "with")
    {
        std::cout << "Usage: OPEN [container] WITH [item]\n";
        return;
    }

    // check if container exists in loc or inv
    auto container = game.graph.locations[game.player.getCurrentLocation()]->findEntityByName(containerName);
    if (!container)
    {
        container = game.player.findEntityInInventory(containerName);
    }

    if (!container)
    {
        std::cout << "You don't see a " << containerName << " here.\n";
        return;
    }

    // find the key in player's inventory
    auto key = game.player.findEntityInInventory(keyName);
    if (!key)
    {
        // else err
        std::cout << "You don't have a " << keyName << ".\n";
        return;
    }

    // send messages using the entity id's  
    game.dispatcher.sendMessage({"player", container->getId(), "unlock", key->getName()});
    game.dispatcher.sendMessage({"player", container->getId(), "open"});
}

// applies the effects of an item
void UseCommand::execute(Game &game, const std::string &args)
{
    std::istringstream iss(args);
    std::string itemName, onKeyword, targetName;

    iss >> itemName;
    itemName = toLowerCase(trim(itemName));

    if (iss >> onKeyword)
    {
        onKeyword = toLowerCase(trim(onKeyword));
        iss >> targetName;
        targetName = toLowerCase(trim(targetName));
    }

    // if there is no keyword or target, use the item on the player
    // by sending a "use" message to the item entity
    // defunct: previouus idea had the ability to use on other entities (ignite)
    if (onKeyword.empty())
    {
        // find the entity in the player's inventory
        auto item = game.player.findEntityInInventory(itemName);
        if (!item)
        {
            std::cout << "You don't have a " << itemName << " to use.\n";
            return;
        }
        // use the item on the player using the actual entity name
        game.dispatcher.sendMessage({"player", item->getId(), "use"});
    }
    else if (onKeyword == "on" && !targetName.empty())
    {
        // use the item on a specific target
        game.dispatcher.sendMessage({"player", itemName, "use", targetName});
    }
    else
    {
        std::cout << "Usage: USE [item] ON [target]\n";
    }
}