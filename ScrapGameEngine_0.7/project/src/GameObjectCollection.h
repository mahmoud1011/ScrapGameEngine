#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "GameObject.h"

namespace ScrapGameEngine
{
	/**
	 * @class GameObjectCollection
	 * @brief Manages a collection of GameObjects in the game engine.
	 *
	 * The `GameObjectCollection` is a globally-scoped utility that provides functionalities
	 * to add, update, render, and dispose of `GameObject` instances. It also supports
	 * efficient lookup of objects by name.
	 */
	class GameObjectCollection
	{
	public:
		/** @brief Default constructor is deleted to prevent instantiation. */
		GameObjectCollection() = delete;

		/**
		 * @brief Adds a new `GameObject` to the collection.
		 * @param go Pointer to the `GameObject` to be added.
		 */
		static void add(GameObject* go);

		/**
		 * @brief Updates all `GameObject` instances in the collection.
		 * @param deltaTime The time elapsed since the last update.
		 */
		static void update(float deltaTime);

		/** @brief Renders all `GameObject` instances in the collection. */
		static void render();

		/** @brief Disposes of all `GameObject` instances in the collection. */
		static void dispose();

		/**
		 * @brief Finds a `GameObject` by its name.
		 * @param name The name of the `GameObject` to search for.
		 * @return A pointer to the found `GameObject`, or `nullptr` if not found.
		 */
		static GameObject* find(std::string name);

	private:
		/**
		 * @brief Stores all active `GameObject` instances.
		 *
		 * This vector is used for managing the lifecycle of all active `GameObject` instances
		 * in the scene.
		 */
		static std::vector<GameObject*> gameObjects;

		/**
		 * @brief Stores `GameObject` instances that need to be added in the next frame.
		 *
		 * This unordered set ensures that duplicate adds are avoided and components are added
		 * safely across frames.
		 */
		static std::unordered_set<GameObject*> gameObjectsToAdd;

		/**
		 * @brief Maps `GameObject` names to their respective pointers for fast lookup.
		 */
		static std::unordered_map<std::string, GameObject*> gameObjectMap;
	};
}
