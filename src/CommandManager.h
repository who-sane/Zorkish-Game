#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include "Command.h"
#include <unordered_map>
#include <memory>
#include <string>

class Game;

class CommandManager
{
public:
    void registerCommand(const std::string &name, std::unique_ptr<Command> command);
    void addAlias(const std::string &alias, const std::string &originalCommand);
    void executeCommand(const std::string &name, Game &game, const std::string &args);
    bool CommandManager::commandExists(const std::string &name) const
    {
        return commands.find(name) != commands.end();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Command>> commands;
    std::unordered_map<std::string, std::string> aliases;
};

#endif
