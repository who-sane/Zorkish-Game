#include "Game.h"
#include "Command.h"
#include "filesystem"
#include <iostream>
#include <sstream>
#include <algorithm>

// helper func to trim whitespace from a string
static std::string trim(const std::string &str)
{
    auto start = str.find_first_not_of(" \t\n\r");
    auto end = str.find_last_not_of(" \t\n\r");
    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// init game with commands, loads adventure file
Game::Game(const std::string &filename)
    : graph(dispatcher), player(1, graph, dispatcher), worldName(extractWorldName(filename)), dispatcher()
{
    std::cout << "Game constructor called." << std::endl;

    // registers commands
    commandManager.registerCommand("go", std::make_unique<GoCommand>());
    commandManager.registerCommand("help", std::make_unique<HelpCommand>());
    commandManager.registerCommand("inventory", std::make_unique<InventoryCommand>());
    commandManager.registerCommand("look", std::make_unique<LookCommand>());
    commandManager.registerCommand("take", std::make_unique<TakeCommand>());
    commandManager.registerCommand("alias", std::make_unique<AliasCommand>());
    commandManager.registerCommand("debug", std::make_unique<DebugTreeCommand>());
    commandManager.registerCommand("look in", std::make_unique<LookInCommand>());
    commandManager.registerCommand("quit", std::make_unique<QuitCommand>());
    commandManager.registerCommand("put", std::make_unique<PutCommand>());
    commandManager.registerCommand("open", std::make_unique<OpenCommand>());
    commandManager.registerCommand("use", std::make_unique<UseCommand>());

    // loads adventure file and displays welcome message
    std::cout << "Loading adventure file: " << filename << std::endl;
    graph.loadFromFile(filename);
    std::cout << "Adventure file loaded." << std::endl;
    std::cout << "-- Welcome Player!! --\n\n ---------------------------------------------------- \n | Currently you're in the world of: " << worldName << "! |\n ----------------------------------------------------\n";
}

// helper func to grab world name from path
std::string Game::extractWorldName(const std::string &filename)
{
    return std::filesystem::path(filename).stem().string();
}

// main game loop
void Game::run()
{
    std::string command;

    // shows initial location to player
    player.displayCurrentLocation();

    // continuously reads player input
    while (true)
    {
        std::cout << "\n> ";
        std::getline(std::cin, command);

        processUInput(command);
    }
}

void Game::processUInput(const std::string &command)
{
    std::istringstream iss(command);
    std::string cmd, args;
    iss >> cmd;
    std::getline(iss, args);

    // case insensitivity
    cmd = toLowerCase(cmd);
    args = trim(args);

    if (cmd == "look")
    {
        if (args.empty()) // if arguments (anything after look) is empty then
        {
            commandManager.executeCommand("look", *this, args); // exec look command
        }
        else if (args.rfind("at ", 0) == 0) // if followed by "at"
        {
            commandManager.executeCommand("look", *this, args); // pass "look" command with "at [the argument or in this case; entity]"
        }
        else if (args.rfind("in ", 0) == 0) // starts with "in " then
        {
            commandManager.executeCommand("look in", *this, args.substr(3)); // pass "look in" command with the argument after "in "
            // subtracting 3 to remove "in " from the string (ie. "in chest" -> "chest")
        }
        else
        {
            std::cout << "Invalid look command. Use 'look' for the location, 'look at [entity]', or 'look in [container]'.\n";
        }
    }
    // handles 'go' or 'move' command with location display after
    else if (cmd == "go" || cmd == "move")
    {
        commandManager.executeCommand(cmd, *this, args);
        player.displayCurrentLocation();
    }
    else
    {
        // executes other commands
        commandManager.executeCommand(cmd, *this, args);
    }
}
