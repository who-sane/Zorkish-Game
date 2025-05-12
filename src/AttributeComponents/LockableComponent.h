#pragma once
#include "../Component.h"
#include <string>

// lockable component for entities requiring a key to unlock
class LockableComponent : public Component
{
public:
    LockableComponent(const std::string &requiredKey) : key(requiredKey), locked(true) {} // init with key and locked state

    bool isLocked() const { return locked; } // check if locked
    void unlock(const std::string &keyUsed)
    { // unlock if key matches
        if (keyUsed == key)
            locked = false;
    }
    void lock() { locked = true; }             // lock entity
    std::string getKey() const { return key; } // get required key

private:
    std::string key; // required key to unlock
    bool locked;     // lock state
};
