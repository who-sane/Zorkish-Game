# Zorkish Game

Zorkish Game is a personal project that reimagines the classic Zork text-based adventure game with a unique twist. Built entirely in native C++, this project showcases a custom implementation of game mechanics, world-building, and interactive storytelling.

## Features
- **Text-Based Adventure**: Navigate through a world using text commands.
- **Custom Components**: Includes unique components like health, lockable objects, and functional items (e.g., containers, openable objects, takeable items, and usable items).
- **Message Dispatching**: Implements a message dispatcher for efficient communication between game entities.
- **Expandable World**: Easily add new locations and interactions through modular design.

## Relevant Files
- `src/`: Contains the core game logic and components.
  - `Command.cpp`, `CommandManager.cpp`: Handle player commands.
  - `Game.cpp`: Main game loop and logic.
  - `Player.cpp`: Player-related functionality.
  - `world/`: Includes example world data for the game.

## How to Run
1. Ensure you have a C++ compiler installed (e.g., GCC or MSVC).
2. Compile the project using your preferred build system or directly via the command line.
3. Run the resulting executable to start the game.

## Inspiration
This project is inspired by the original Zork game, with added features and mechanics to make it a unique experience.