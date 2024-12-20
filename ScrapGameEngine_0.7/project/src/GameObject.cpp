#include "GameObject.h"
#include <algorithm> 
using namespace ScrapGameEngine;

// Static factory method to create a GameObject with a default name
GameObject* GameObject::Create()
{
    GameObject* newObj = new GameObject();
    return newObj;
}

// Static factory method to create a GameObject with a specified name
GameObject* GameObject::Create(const std::string& name)
{
    GameObject* newObj = new GameObject(name);
    return newObj;
}

// Default constructor
GameObject::GameObject() : name("New GameObject")
{
    transform = addComponent<Transform>(); 
}

// Constructor that takes a name parameter
GameObject::GameObject(const std::string& name) : name(name)
{
    transform = addComponent<Transform>(); 
    if (!transform) {
        std::cerr << "[GameObject] Error: Transform failed to initialize!" << std::endl;
    }
}

GameObject::~GameObject()
{
    for (auto component : components)
    {
        if (component != nullptr) {
            delete component; // Ensure no double-deletion
        }
        else {
            std::cout << "[GameObject] Null component detected during cleanup!" << std::endl;
        }
    }
    std::cout << "[GameObject] GameObject " << name << " deleted" << std::endl;
}

void GameObject::runComponentAwake()
{
    if (componentsJustAdded.empty()) return;

    for (auto newComponent : componentsJustAdded)
    {
        newComponent->awake();
    }
}

void GameObject::runComponentStart()
{
    if (componentsJustAdded.empty()) return;

    for (auto newComponent : componentsJustAdded)
    {
        newComponent->start();
    }

    componentsJustAdded.clear();
}

void GameObject::runComponentUpdate(float deltaTime)
{
    auto it = components.begin();
    while (it != components.end())
    {
        if ((*it)->shouldDestroy())
        {
            delete* it;
            it = components.erase(it);
        }
        else
        {
            (*it)->update(deltaTime);
            ++it;
        }
    }
}

void GameObject::runComponentRender()
{
    for (auto component : components)
    {
        component->render();
    }
}

void GameObject::setName(std::string value)
{
    name = value;
}

std::string GameObject::getName() const
{
    return name;
}

void GameObject::destroy()
{
    flaggedForDeletion = true;
}

bool GameObject::shouldDestroy() const
{
    return flaggedForDeletion;
}
