#include "GameObjectCollection.h"
#include <iostream>
using namespace ScrapGameEngine; 

std::vector<GameObject*> GameObjectCollection::gameObjects;
std::unordered_set<GameObject*> GameObjectCollection::gameObjectsToAdd;
std::unordered_map<std::string, GameObject*> GameObjectCollection::gameObjectMap;

void GameObjectCollection::add(GameObject* go)
{
	// Only add to gameObjectsToAdd if the object is not already in the set
	if (gameObjectsToAdd.find(go) == gameObjectsToAdd.end())
	{
		gameObjectsToAdd.insert(go);

		// Check if the GameObject is already in the gameObjects vector
		// Only add if it's not found in the map (faster check)
		if (gameObjectMap.find(go->getName()) == gameObjectMap.end())
		{
			gameObjects.push_back(go);
			gameObjectMap[go->getName()] = go; // Add to map for fast lookup by name
		}
	}
}

void GameObjectCollection::update(float deltaTime)
{
	// Safely remove objects flagged for deletion
	gameObjects.erase(std::remove_if(gameObjects.begin(), gameObjects.end(),
		[](GameObject* go) { return go->shouldDestroy(); }), gameObjects.end());

	// Add new game objects if there are any pending
	if (!gameObjectsToAdd.empty())
	{
		// Copy new objects to local list to avoid iterator invalidation
		std::vector<GameObject*> objectsToAdd(gameObjectsToAdd.begin(), gameObjectsToAdd.end());

		for (auto* go : objectsToAdd)
		{
			go->runComponentAwake();
			go->runComponentStart();
		}

		gameObjects.insert(gameObjects.end(), objectsToAdd.begin(), objectsToAdd.end());
		gameObjectsToAdd.clear();  // Safely clear after processing
	}

	// Update existing objects
	for (auto* go : gameObjects)
	{
		go->runComponentUpdate(deltaTime);
	}
}

void GameObjectCollection::render()
{
	// For all element in gameObjects, run its render() fn
	// For all elements in gameObjects, run their render function
	for (auto* go : gameObjects)
	{
		go->runComponentRender();
	}
}

// This function is to be called when the current scene is being deactivated or destroyed.
// With SceneStateMachine:
// - current scene is deactivated before changing to a different scene.
void GameObjectCollection::dispose()
{
	//// Delete all elements in gameObjects
	//for (auto* go : gameObjects)
	//{
	//	delete go;
	//}

	// Clear gameObjects and gameObjectsToAdd
	gameObjects.clear();
	gameObjectsToAdd.clear();
	gameObjectMap.clear();  // Clear the map as well
}

GameObject* GameObjectCollection::find(std::string name)
{
	// Find the game object by name using the map (fast lookup)
	auto it = gameObjectMap.find(name);
	if (it != gameObjectMap.end())
	{
		return it->second;
	}
	return nullptr; // Return nullptr if no match is found
}