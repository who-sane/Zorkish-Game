#ifndef GAME_H
#define GAME_H

#include "Graph.h"
#include "Player.h"
#include "CommandManager.h"
#include "MessageDispatcher.h"
#include <string>

class Game
{
public:
    Game(const std::string &filename); 
    void run();
    void processUInput(const std::string &command); 

    MessageDispatcher dispatcher; 
    Graph graph;
    Player player;
    std::string worldName;
    CommandManager commandManager;

private:
    std::string extractWorldName(const std::string &filename);
};

#endif
