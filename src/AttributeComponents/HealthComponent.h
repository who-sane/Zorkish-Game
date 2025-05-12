#pragma once
#include "../Component.h"

// health component to track and modify health value
class HealthComponent : public Component {
public:
    HealthComponent(int initialHealth) : health(initialHealth) {} // initialize health with given value

    int getHealth() const { return health; } // get current health
    void modifyHealth(int amount) { health += amount; } // adjust health by specified amount

private:
    int health; // current health value
};
