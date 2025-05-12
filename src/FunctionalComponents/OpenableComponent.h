#pragma once
#include "../Component.h"

class OpenableComponent : public Component
{
public:
    // init
    OpenableComponent(bool initiallyOpen = false) : open(initiallyOpen) {}

    // checks if open
    bool isOpen() const { return open; }

    // setterz
    void setOpen() { open = true; } 
    void setClosed() { open = false; } 

private:
    bool open; // stores open state
};
