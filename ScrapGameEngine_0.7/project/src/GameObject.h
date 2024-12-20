#pragma once
#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include "BaseComponent.h"
#include <algorithm>
#include <type_traits>
#include "Transform.h"
#include <iostream>

namespace ScrapGameEngine
{
	class Transform;

	/**
	 * @class GameObject
	 * @brief Represents an entity in the game world that can contain components.
	 *
	 * The GameObject class serves as the base entity in the game engine. It can hold multiple
	 * components that define its behavior and properties. Each GameObject has a unique name,
	 * a transform for spatial information, and methods to manage its lifecycle.
	 */
	class GameObject
	{
	public:
		/**
		 * @brief Creates a new GameObject instance.
		 * @return A pointer to the newly created GameObject.
		 */
		static GameObject* Create();

		/**
		 * @brief Creates a new GameObject instance with a specified name.
		 * @param name The name of the GameObject.
		 * @return A pointer to the newly created GameObject.
		 */
		static GameObject* Create(const std::string& name);

		/** @brief Pointer to the transform component of the GameObject. */
		Transform* transform;

		/** @brief Default constructor for GameObject. */
		GameObject();

		/**
		 * @brief Constructor for GameObject with a name.
		 * @param name The name of the GameObject.
		 */
		GameObject(const std::string& name);

		/** @brief Destructor for GameObject. */
		~GameObject();

		/** @brief Calls the awake method on all attached components. */
		void runComponentAwake();

		/** @brief Calls the start method on all attached components. */
		void runComponentStart();

		/**
		 * @brief Calls the update method on all attached components.
		 * @param deltaTime The time elapsed since the last update.
		 */
		void runComponentUpdate(float deltaTime);

		/** @brief Calls the render method on all attached components. */
		void runComponentRender();

		/**
		 * @brief Adds a new component of type T to the GameObject.
		 * @tparam T The type of component to add (must inherit from BaseComponent).
		 * @return A pointer to the newly added component.
		 */
		template <
			typename T,
			typename = typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type
		>
		T* addComponent()
		{
			// Create instance of T, expecting that T has a constructor that takes GameObject*
			T* newComponent = new T(this);

			// Add the instance of T to componentsJustAdded
			componentsJustAdded.push_back(newComponent);

			// Also add it to components
			components.push_back(newComponent);

			return newComponent;
		}

		/**
		 * @brief Retrieves a component of type T from the GameObject.
		 * @tparam T The type of component to retrieve (must inherit from BaseComponent).
		 * @return A pointer to the component of type T, or nullptr if not found.
		 */
		template <
			typename T,
			typename = typename std::enable_if<std::is_base_of<BaseComponent, T>::value>::type
		>
		T* getComponent()
		{
			// Loop through all components and return the first one that matches type T
			for (BaseComponent* component : components)
			{
				T* result = dynamic_cast<T*>(component);
				if (result != nullptr)
				{
					return result; // Return the first found component of type T
				}
			}
			return nullptr; // Return nullptr if no component of type T is found
		}

		/**
		 * @brief Sets the name of the GameObject.
		 * @param value The new name for the GameObject.
		 */
		void setName(std::string value);

		/**
		 * @brief Retrieves the name of the GameObject.
		 * @return The name of the GameObject.
		 */
		std::string getName() const;

		/** @brief Flags the GameObject for destruction. */
		void destroy();

		/**
		 * @brief Checks if the GameObject is flagged for destruction.
		 * @return True if flagged for destruction, false otherwise.
		 */
		bool shouldDestroy() const;

	protected:
		/** @brief The name of the GameObject. */
		std::string name;

	private:
		/** @brief A list of components attached to the GameObject. */
		std::vector<BaseComponent*> components;

		/** @brief A list of components added in the current frame. */
		std::vector<BaseComponent*> componentsJustAdded;

		/** @brief Indicates whether the GameObject is flagged for deletion. */
		bool flaggedForDeletion = false;
	};
}
