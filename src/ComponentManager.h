#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include "Component.h"

class ComponentManager {
public:
    // adds a component to the entity
    template <typename T>
    void addComponent(std::shared_ptr<T> component) {
        components[std::type_index(typeid(T))] = component;
    }

    // retrieve a component if it exists, else returns nullptr
    template <typename T>
    std::shared_ptr<T> getComponent() const {
        auto it = components.find(std::type_index(typeid(T)));
        if (it != components.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    // check if entity has a specific component
    template <typename T>
    bool hasComponent() {
        return components.count(std::type_index(typeid(T))) > 0;
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<Component>> components;
};
