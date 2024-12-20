#include "BaseScene.h"
#include "GameObjectCollection.h"

using namespace ScrapGameEngine;

// Called first when changed to this scene.
void BaseScene::initialize()
{
    onInitialize();
}

// Called second when changed to this scene.
void BaseScene::activate()
{
    onActivate();
}

// Called before changing to a different scene.
void BaseScene::deactivate()
{
    // Dispose all gameObjects
    GameObjectCollection::dispose(); 
    onDeactivate();
}

void BaseScene::update(float deltaTime)
{
    // Update all gameObjects
    GameObjectCollection::update(deltaTime); 
    onUpdate(deltaTime);
}

void BaseScene::render()
{
    // Render all gameObjects
    GameObjectCollection::render(); 
    onRender();
}
