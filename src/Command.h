#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <iostream>

class Game; // Forward declaration to avoid circular dependencies

// Base Command class
class Command
{
public:
    // https://www.quantstart.com/articles/C-Virtual-Destructors-How-to-Avoid-Memory-Leaks/#:~:text=In%20simple%20terms%2C%20a%20virtual,known%20as%20a%20memory%20leak.
    // the command destructor is virtual to allow derived classes to be destroyed correctly
    virtual ~Command() = default;
    virtual void execute(Game &game, const std::string &args) = 0;
};

class GoCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class HelpCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class InventoryCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class LookCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class AliasCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class DebugTreeCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class QuitCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class LookInCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class TakeCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class PutCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

class OpenCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};
class UseCommand : public Command
{
public:
    void execute(Game &game, const std::string &args) override;
};

#endif