#pragma once
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "BaseComponent.h"

namespace ScrapGameEngine
{
    class GameObject; // Forward declaration to avoid circular dependencies

    /**
     * @class Transform
     * @brief Represents the transformation of a game object, including position, rotation, and scale.
     *
     * The Transform class provides methods to manipulate the position, rotation, and scale of a game object.
     * It also supports parenting and hierarchical transformations, where a parent transform can control its children.
     */
    class Transform : public BaseComponent
    {
    public:
        /**
         * @brief Constructor that initializes the Transform component for a game object.
         * @param owner Pointer to the associated GameObject.
         */
        Transform(GameObject* owner);

        /**
         * @brief Retrieves the world position of the game object.
         * @return The world position as a glm::vec2.
         */
        glm::vec2 getWorldPosition() const;

        /**
         * @brief Retrieves the world rotation of the game object.
         * @return The world rotation in degrees.
         */
        float getWorldRotation() const;

        /**
         * @brief Retrieves the local position of the game object.
         * @return The local position as a glm::vec2.
         */
        glm::vec2 getPosition() const;

        /**
         * @brief Retrieves the local rotation of the game object.
         * @return The local rotation in degrees.
         */
        float getLocalRotation() const;

        /**
         * @brief Retrieves the local scale of the game object.
         * @return The local scale as a glm::vec2.
         */
        glm::vec2 getLocalScale() const;

        /**
         * @brief Sets the position of the game object.
         * @param position The new position as a glm::vec2.
         */
        void setPosition(const glm::vec2& position);

        /**
         * @brief Sets the rotation of the game object.
         * @param rotation The new rotation in degrees.
         */
        void setRotation(float rotation);

        /**
         * @brief Sets the scale of the game object.
         * @param scale The new scale as a glm::vec2.
         */
        void setScale(const glm::vec2& scale);

        /**
         * @brief Sets the parent transform for this transform.
         * @param newParent Pointer to the new parent Transform.
         */
        void setParent(Transform* newParent);

        /**
         * @brief Gets the parent transform.
         * @return Pointer to the parent Transform.
         */
        Transform* getParent() const;

        /**
         * @brief Gets all child transforms.
         * @return A vector of pointers to child Transforms.
         */
        const std::vector<Transform*>& getChildren() const;

    private:
        glm::vec2 localPosition;  ///< Local position relative to parent transform
        float localRotation;      ///< Local rotation in degrees relative to parent transform
        glm::vec2 localScale;     ///< Local scale relative to parent transform

        Transform* parent = nullptr; ///< Pointer to the parent transform
        std::vector<Transform*> children; ///< List of child transforms

        /**
         * @brief Calculates the world position based on local transformations and parent transformations.
         * @return The calculated world position as a glm::vec2.
         */
        glm::vec2 calculateWorldPosition() const;

        /**
         * @brief Calculates the world rotation based on local transformations and parent transformations.
         * @return The calculated world rotation in degrees.
         */
        float calculateWorldRotation() const;

        /**
         * @brief Calculates the world scale based on local transformations and parent transformations.
         * @return The calculated world scale as a glm::vec2.
         */
        glm::vec2 calculateWorldScale() const;
    };
}
