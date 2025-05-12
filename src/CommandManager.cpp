#include "CommandManager.h"
#include "Game.h"

// registers a command with the manager
void CommandManager::registerCommand(const std::string &name, std::unique_ptr<Command> command)
{
    commands[name] = std::move(command);
}

// maps an alias to an existing command
void CommandManager::addAlias(const std::string &alias, const std::string &originalCommand)
{
    // check if the original command exists in the command list
    if (commandExists(originalCommand))
    {
        // if the original command exists, add alias to aliases map
        aliases[alias] = originalCommand;
        std::cout << "Alias created: " << alias << " -> " << originalCommand << "\n";
    }
    else
    {
        // if the original command does not exist, print an error message
        std::cerr << "Error: cannot alias nonexistent command '" << originalCommand << "'.\n";
    }
}

// executes a command
void CommandManager::executeCommand(const std::string &name, Game &game, const std::string &args)
{
    std::string commandName = name;

    // resolves alias if it exists
    if (aliases.find(commandName) != aliases.end())
    {
        commandName = aliases[commandName];
    }

    // executes the resolved command if it exists
    if (commands.find(commandName) != commands.end())
    {
        commands[commandName]->execute(game, args);
    }
    else
    {
        std::cout << "Invalid command.\n";
    }
}
