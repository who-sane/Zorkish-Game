#ifndef ENTITY_H
#define ENTITY_H
#include "./FunctionalComponents/TakeableComponent.h"
#include "./FunctionalComponents/ContainerComponent.h"
#include "./FunctionalComponents/OpenableComponent.h"
#include "./FunctionalComponents/UseableComponent.h"
#include "./AttributeComponents/LockableComponent.h"
#include "./AttributeComponents/HealthComponent.h"
#include "ComponentManager.h"
#include "MessageDispatcher.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

class Entity : public std::enable_shared_from_this<Entity>
{
public:
    // constructor with basic properties
    Entity(const std::string &name, const std::string &description, MessageDispatcher &dispatcher)
        : name(name), description(description), dispatcher(dispatcher), id(name) 
    {
        // create a unique ID via timestamp and random number
        // using the 'time'  and 'rand' function from Standard C++ Library 
        std::string uniqueId = name + "_" + std::to_string(std::time(nullptr));
        while (!dispatcher.registerRecipient(uniqueId, [this](const Message &msg)
                                             { handleMessage(msg); }))
        {
            uniqueId = name + "_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(rand());
        }
        id = uniqueId; // on success store id as the uniqueID
    }

    static std::string toLowerCase(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c)
                       { return std::tolower(c); });
        return result;
    }

    // getters
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::string getId() const { return id; } 

    // template method to retrieve a component
    template <typename T>
    std::shared_ptr<T> getComponent() const
    {
        return componentManager.getComponent<T>();
    }

    // template method to add a component to the entity
    template <typename T>
    void addComponent(std::shared_ptr<T> component)
    {
        componentManager.addComponent(component);
    }

    // adds an entity to another entity if it's a container
    void addContainedEntity(std::shared_ptr<Entity> entity)
    {
        if (auto container = getComponent<ContainerComponent>())
        {
            container->addItem(entity);
        }
    }

    // retrieves entities within this container, if any
    const std::vector<std::shared_ptr<Entity>> &getContainedEntities() const
    {
        if (auto container = getComponent<ContainerComponent>())
        {
            return container->getContents();
        }
        static const std::vector<std::shared_ptr<Entity>> empty; // return empty if not a container
        return empty;
    }

    // removes a contained entity from this container
    void removeContainedEntity(std::shared_ptr<Entity> entity)
    {
        if (auto container = getComponent<ContainerComponent>())
        {
            container->removeItem(entity);
        }
    }

    // sends a message via the dispatcher
    void sendMessage(const std::string &to, const std::string &message, std::any data = {})
    {
        std::cout << "Entity '" << name << "' sending message to '" << to << "' with message: '" << message << "'\n";
        dispatcher.sendMessage({id, to, message, data});
    }

    // handles incoming messages
    // adaptation of existing methods 
    void handleMessage(const Message &msg)
    {
        std::cout << "Entity '" << name << "' received message from '" << msg.from << "' with message: '" << msg.message << "'\n";
        if (msg.message == "inspect")
        {
            std::cout << "The " << name << " is inspected: " << description << "\n";
        }
        else if (msg.message == "use")
        {
            if (auto usable = getComponent<UsableComponent>())
            {
                if (usable->getEffectType() == UseEffectType::HEAL)
                {
                    std::cout << "You feel rejuvenated after using the " << name << ".\n";
                    dispatcher.sendMessage({id, "player", "heal", usable->getEffectValue()});
                }
                else if (usable->getEffectType() == UseEffectType::DAMAGE)
                {
                    std::cout << "You feel a burning sensation as you consume the " << name << ".\n";
                    dispatcher.sendMessage({id, "player", "damage", std::abs(usable->getEffectValue())});
                }
                dispatcher.sendMessage({id, "player", "removeItem", shared_from_this()});
            }
            else
            {
                std::cout << "You can't use " << name << ".\n";
            }
        }
        else if (msg.message == "unlock")
        {
            if (auto lockable = getComponent<LockableComponent>())
            {
                try
                {
                    lockable->unlock(std::any_cast<std::string>(msg.data));
                    std::cout << name << " has been unlocked.\n";
                }
                catch (const std::bad_any_cast &)
                {
                    std::cerr << "Invalid key data for unlocking " << name << ".\n";
                }
            }
        }
        else if (msg.message == "open")
        {
            if (auto openable = getComponent<OpenableComponent>())
            {
                if (!openable->isOpen())
                {
                    openable->setOpen();
                    std::cout << name << " is now open.\n";
                }
                else
                {
                    std::cout << name << " is already open.\n";
                }
            }
        }
        else if (msg.message == "close")
        {
            if (auto openable = getComponent<OpenableComponent>())
            {
                if (openable->isOpen())
                {
                    openable->setClosed();
                    std::cout << name << " is now closed.\n";
                }
                else
                {
                    std::cout << name << " is already closed.\n";
                }
            }
        }
        else if (msg.message == "addItem")
        {
            if (auto container = getComponent<ContainerComponent>())
            {
                container->addItem(std::any_cast<std::shared_ptr<Entity>>(msg.data));
                std::cout << "Item added to " << name << ".\n";
            }
        }
        else if (msg.message == "removeItem")
        {
            if (auto container = getComponent<ContainerComponent>())
            {
                container->removeItem(std::any_cast<std::shared_ptr<Entity>>(msg.data));
                std::cout << "Item removed from " << name << ".\n";
            }
        }
        else if (msg.message == "look_in")
        {
            if (auto container = getComponent<ContainerComponent>())
            {
                if (auto openable = getComponent<OpenableComponent>())
                {
                    if (!openable->isOpen())
                    {
                        std::cout << name << " is closed.\n";
                        return;
                    }
                }

                const auto &contents = container->getContents();
                if (contents.empty())
                {
                    std::cout << "The " << name << " is empty.\n";
                }
                else
                {
                    std::cout << "Inside the " << name << " you find:\n";
                    for (const auto &item : contents)
                    {
                        std::cout << " - " << item->getName() << ": " << item->getDescription() << "\n";
                    }
                }
            }
            else
            {
                std::cout << "You can't look inside " << name << ".\n";
            }
        }
        else if (msg.message == "take_from")
        {
            if (auto container = getComponent<ContainerComponent>())
            {
                if (auto lockable = getComponent<LockableComponent>())
                {
                    if (lockable->isLocked())
                    {
                        std::cout << name << " is locked.\n";
                        return;
                    }
                }

                if (auto openable = getComponent<OpenableComponent>())
                {
                    if (!openable->isOpen())
                    {
                        std::cout << "The " << name << " is closed.\n";
                        return;
                    }
                }

                std::string itemName = std::any_cast<std::string>(msg.data);
                const auto &contents = container->getContents();
                for (const auto &item : contents)
                {
                    if (toLowerCase(item->getName()) == toLowerCase(itemName))
                    {
                        if (!item->getComponent<TakeableComponent>())
                        {
                            std::cout << "You can't take that.\n";
                            return;
                        }
                        container->removeItem(item);
                        dispatcher.sendMessage({id, "player", "addItem", item});
                        std::cout << "Taken " << item->getName() << " from " << name << ".\n";
                        return;
                    }
                }
                std::cout << "You don't see a " << itemName << " in there.\n";
            }
            else
            {
                std::cout << name << " is not a container.\n";
            }
        }
        else if (msg.message == "put_item")
        {
            if (auto container = getComponent<ContainerComponent>())
            {
                if (auto lockable = getComponent<LockableComponent>())
                {
                    if (lockable->isLocked())
                    {
                        std::cout << name << " is locked.\n";
                        return;
                    }
                }

                if (auto openable = getComponent<OpenableComponent>())
                {
                    if (!openable->isOpen())
                    {
                        std::cout << "The " << name << " is closed.\n";
                        return;
                    }
                }

                auto item = std::any_cast<std::shared_ptr<Entity>>(msg.data);
                container->addItem(item);
                dispatcher.sendMessage({id, "player", "removeItem", item});
                std::cout << "You put the " << item->getName() << " in the " << name << ".\n";
            }
            else
            {
                std::cout << "The " << name << " is not a container.\n";
            }
        }
    }

private:
    std::string id;                    // unique identifier for the entity
    std::string name;                  // display name of the entity
    std::string description;           // description of the entity
    ComponentManager componentManager; // manages components of the entity
    MessageDispatcher &dispatcher;     // reference to the global dispatcher
    bool takeable;
    bool flammable;
};

#endif
