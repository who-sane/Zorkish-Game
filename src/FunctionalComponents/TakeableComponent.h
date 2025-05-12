#pragma once
#include "../Component.h"

class TakeableComponent : public Component {
public:
    TakeableComponent() = default;
    bool isTakeable() const { return true; }
};
