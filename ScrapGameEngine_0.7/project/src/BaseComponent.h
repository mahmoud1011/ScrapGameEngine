#pragma once

namespace ScrapGameEngine
{
    /**
     * @class BaseComponent
     * @brief Represents a base class for all components attached to a GameObject in the ScrapGameEngine.
     *
     * The `BaseComponent` class provides the fundamental interface for all game components,
     * such as rendering, updating, and initialization. It holds a reference to its parent `GameObject`.
     * Components can be added, updated, and removed as needed throughout the game.
     */
    class GameObject; // Forward declaration that GameObject class exists.

    class BaseComponent
    {
    public:
        /**
         * @brief Constructs a new BaseComponent with a reference to a GameObject.
         * @param go Pointer to the GameObject this component is attached to.
         */
        BaseComponent(GameObject* go);

        /**
         * @brief Virtual destructor.
         *
         * The destructor ensures that any derived classes are properly cleaned up when the base
         * component is destroyed.
         */
        virtual ~BaseComponent() = 0;

        /**
         * @brief Initializes the component.
         *
         * Called once when the component is first added to a GameObject. Use this for initial setup.
         */
        virtual void awake();

        /**
         * @brief Starts the component.
         *
         * Called after `awake()`. Use this for setting up initial states.
         */
        virtual void start();

        /**
         * @brief Updates the component every frame.
         * @param deltaTime Time elapsed since the last frame.
         *
         * Implement this function to include the logic that updates the component state each frame.
         */
        virtual void update(float deltaTime);

        /**
         * @brief Renders the component.
         *
         * Implement this function to include the logic that renders the component's visual representation.
         */
        virtual void render();

        /**
         * @brief Marks the component for deletion.
         */
        void destroy();

        /**
         * @brief Checks if the component is marked for deletion.
         * @return `true` if the component is flagged for deletion, `false` otherwise.
         */
        bool shouldDestroy() const;

    protected:
        GameObject* gameObject; ///< Pointer to the GameObject this component is attached to.
        bool flaggedForDeletion = false; ///< Flag to indicate if the component is scheduled for deletion.
    };
}
