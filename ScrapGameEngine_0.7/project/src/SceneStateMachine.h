#pragma once
#include "BaseScene.h"
#include <unordered_map>
#include <iostream>

namespace ScrapGameEngine
{
    /**
     * @class SceneStateMachine
     * @brief Manages scenes within the game engine, enabling the addition, loading, and management of scenes.
     *
     * The SceneStateMachine is a static class that provides functionality for handling game scenes.
     * It supports adding new scenes, loading scenes by name or index, and accessing the current scene.
     */
    class SceneStateMachine
    {
    public:
        /**
         * @brief Deleted default constructor to prevent instantiation of the SceneStateMachine class.
         */
        SceneStateMachine() = delete;

        /**
         * @brief Adds a new scene to the scene manager.
         *
         * This function is a template and only allows types derived from BaseScene.
         * If a scene with the same name already exists, the function fails.
         *
         * @tparam T The type of the scene to add (must inherit from BaseScene).
         * @return A pointer to the newly added scene if successful; otherwise, nullptr.
         */
        template<typename T, typename = typename std::enable_if<std::is_base_of<BaseScene, T>::value>::type>
        static T* addScene() {
            T* newScene = new T();

            auto pair = std::make_pair(newScene->getName(), newScene);

            std::pair < std::unordered_map<std::string, BaseScene*>::iterator, bool > result = scenes.insert(pair);

            // if successfully adding the new pair
            if (result.second) {
                std::cout << "Scene '" << newScene->getName() << "' added successfully with ID: " << sceneIdCounter << std::endl;

                // For challenge (see below)
                unsigned int currentId = sceneIdCounter;
                sceneIdCounter++;

                return newScene;
            }
            else {
                std::cout << "Failed to add scene '" << newScene->getName() << "': Scene already exists." << std::endl;
                delete newScene;
                return nullptr;
            }
        }

        /**
         * @brief Loads a scene by its name.
         *
         * @param name The name of the scene to load.
         */
        static void loadScene(const std::string name);

        /**
         * @brief Loads a scene by its index in the scene manager.
         *
         * The index is 0-based and corresponds to the order in which scenes are added.
         *
         * @param index The index of the scene to load.
         */
        static void loadScene(const unsigned int index);

        /**
         * @brief Retrieves the current active scene.
         *
         * @return A pointer to the current active BaseScene.
         */
        static BaseScene* getCurrentScene();

    private:
        /**
         * @brief Updates the current scene.
         *
         * This function is called internally by the engine to update the active scene each frame.
         *
         * @param deltaTime The time elapsed since the last frame.
         */
        static void update(float deltaTime);

        /**
         * @brief Renders the current scene.
         *
         * This function is called internally by the engine to render the active scene each frame.
         */
        static void render();

        /**
         * @brief Disposes of all scenes and cleans up memory.
         *
         * This function is called during engine shutdown to ensure all resources are released.
         */
        static void dispose();

        static std::unordered_map<std::string, BaseScene*> scenes; /**< Map of scene names to their respective BaseScene instances. */
        static BaseScene* currentScene; /**< Pointer to the currently active scene. */
        static unsigned int sceneIdCounter; /**< Counter for assigning unique IDs to scenes. */

        /**
         * @brief Grants the Application class access to private members and methods.
         */
        friend class Application;
    };
}
