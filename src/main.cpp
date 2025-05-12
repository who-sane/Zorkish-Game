#include <filesystem>
#include <iostream>
#include "Game.h"

// To compile (if you’re using cl.exe from MSVC):
//  Navigate to the Zorkish_Adventure/src (directory) in your CLI terminal
//  Run: cl /EHsc /std:c++17 *.cpp /link /out:Zorkish.exe

int main()
{
    //todo: remove debugging from program (commentout)
    std::string filename = "../world/example_world.txt"; 
    if (!std::filesystem::exists(filename))
    {
        std::cerr << "Error: File not found at path: " << filename << std::endl;
        return 1;
    }

    try
    {
        std::cout << "Initialising game with file: " << filename << std::endl;
        Game game(filename);
        std::cout << "Game initialized successfully." << std::endl;
        game.run();
        std::cout << "Game loop ended." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "An unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
