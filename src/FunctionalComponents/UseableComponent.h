#pragma once
#include "../Component.h"

enum class UseEffectType
{
    NONE,  
    HEAL,  // heal effect
    DAMAGE // damage effect
};

class UsableComponent : public Component
{
public:
    // init with type and value
    UsableComponent(UseEffectType effectType = UseEffectType::NONE, int effectValue = 0)
        : effectType(effectType), effectValue(effectValue) {}

    // retrieves effect type
    UseEffectType getEffectType() const { return effectType; }

    // retrieves effect value
    int getEffectValue() const { return effectValue; }

private:
    UseEffectType effectType; // stores effect type
    int effectValue;          // stores effect value
};
