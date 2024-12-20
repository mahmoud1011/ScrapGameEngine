#include "SceneStateMachine.h"
#include "GameObjectCollection.h"
using namespace ScrapGameEngine;

std::unordered_map<std::string, BaseScene*> SceneStateMachine::scenes;
BaseScene* SceneStateMachine::currentScene;
unsigned int SceneStateMachine::sceneIdCounter;

void SceneStateMachine::loadScene(const std::string name)
{
    std::cout << "[SCENE_MANAGER] Loading scene: " << name << std::endl; // Log scene name

    // 1. Look for the scene matching the name
    auto it = scenes.find(name);
    if (it != scenes.end())
    {
        // If found:
        // a. Deactivate currentScene, if there is one
        if (currentScene)
        {
            currentScene->deactivate();
            std::cout << "[SCENE_MANAGER] Deactivated scene: " << currentScene->getName() << std::endl; // Logging
        }

        // b. Dispose all currently active gameObjects.
        GameObjectCollection::dispose();

        // c. Point currentScene to the new scene.
        currentScene = it->second;

        // d. Initialize and activate the new scene.
        currentScene->initialize();
        currentScene->activate();
    }
    else
    {
        // Otherwise, log error
        std::cout << "[SCENE_MANAGER] Error: Scene '" << name << "' not found." << std::endl;
    }
}

void SceneStateMachine::loadScene(const unsigned int index)
{
    std::cout << "[SCENE_MANAGER] Loading scene: " << index << std::endl; // Log scene name

    // Ensure the index is valid
    if (index < scenes.size())
    {
        auto it = std::next(scenes.begin(), index);
        loadScene(it->first);
    }
    else
    {
        std::cout << "[SCENE_MANAGER] Error: Invalid scene index." << std::endl;
    }
}

void SceneStateMachine::update(float deltaTime)
{
    // Update current scene if there is one.
    if (currentScene)
    {
        currentScene->update(deltaTime);
    }
}

void SceneStateMachine::render()
{
    // Render current scene if there is one.
    if (currentScene)
    {
        currentScene->render();
    }
}

void SceneStateMachine::dispose()
{
    // Dispose all currently active gameObjects.
    GameObjectCollection::dispose();

    // For each element in scenes
    for (auto& scene : scenes)
    {
        delete scene.second; // Clean up each scene
        std::cout << "[SCENE_MANAGER] Disposing scene: " << scene.second->getName() << std::endl; // Log scene name
    }
    scenes.clear(); // Clear the collection

    // Reset sceneIdCounter
    sceneIdCounter = 0;

    // Set currentScene to nullptr for safety.
    currentScene = nullptr;
}

BaseScene* SceneStateMachine::getCurrentScene()
{
	return currentScene;
}
