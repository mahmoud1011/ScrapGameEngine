#pragma once
#include <string>
#include <vector>
#include "GameObject.h"

namespace ScrapGameEngine
{
    /**
     * @class BaseScene
     * @brief Represents the base class for all scenes in the ScrapGameEngine.
     *
     * The `BaseScene` class defines the lifecycle of a scene, including initialization,
     * activation, deactivation, updates, and rendering. Derived classes must implement
     * the required functions to provide custom behavior.
     */
    class BaseScene
    {
    protected:
        /**
         * @brief Initializes the scene.
         *
         * This pure virtual function must be implemented by derived classes to
         * provide custom initialization logic for the scene.
         */
        virtual void onInitialize() = 0;

        /**
         * @brief Activates the scene.
         *
         * Called when the scene becomes active. Can be overridden by derived classes
         * to provide custom logic when the scene is activated.
         */
        virtual void onActivate() {};

        /**
         * @brief Deactivates the scene.
         *
         * Called when the scene becomes inactive. Can be overridden by derived classes
         * to provide custom logic when the scene is deactivated.
         */
        virtual void onDeactivate() {};

        /**
         * @brief Updates the scene.
         * @param deltaTime Time elapsed since the last frame.
         *
         * Can be overridden by derived classes to provide custom update logic for the scene.
         */
        virtual void onUpdate(float deltaTime) {};

        /**
         * @brief Renders the scene.
         *
         * Can be overridden by derived classes to provide custom rendering logic for the scene.
         */
        virtual void onRender() {};

    public:
        /**
         * @brief Gets the name of the scene.
         * @return A string representing the name of the scene.
         *
         * This pure virtual function must be implemented by derived classes to return the scene's name.
         */
        virtual std::string getName() const = 0;

        /**
         * @brief Initializes the scene by calling `onInitialize`.
         */
        void initialize();

        /**
         * @brief Activates the scene by calling `onActivate`.
         */
        void activate();

        /**
         * @brief Deactivates the scene by calling `onDeactivate`.
         */
        void deactivate();

        /**
         * @brief Updates the scene by calling `onUpdate`.
         * @param deltaTime Time elapsed since the last frame.
         */
        void update(float deltaTime);

        /**
         * @brief Renders the scene by calling `onRender`.
         */
        void render();
    };
}
