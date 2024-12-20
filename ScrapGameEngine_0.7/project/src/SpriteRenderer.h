#pragma once
#include "BaseComponent.h"
#include "Texture2D.h"
#include "Mesh.h"
#include "GameObject.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ScrapGameEngine {

    /**
     * @class SpriteRenderer
     * @brief Manages the rendering of a 2D sprite in the game.
     *
     * This component handles rendering a 2D sprite, applying color tint, opacity, size,
     * pivot, and texture. It interacts with the underlying `Mesh` and `Texture2D` objects
     * to render the sprite efficiently.
     */
    class SpriteRenderer : public BaseComponent {
    public:
        // Constructor
        SpriteRenderer(GameObject* owner);

        /**
         * @brief Initializes the component.
         */
        void awake() override;

        /**
         * @brief Set the color of the sprite.
         * @param r Red component (0.0 to 1.0).
         * @param g Green component (0.0 to 1.0).
         * @param b Blue component (0.0 to 1.0).
         */
        void setColour(float r, float g, float b);

        /**
         * @brief Set the color of the sprite.
         * @param color A `glm::vec3` representing the RGB color.
         */
        void setColour(const glm::vec3& color);

        /**
         * @brief Set the opacity of the sprite.
         * @param opacity The opacity level (0.0 to 1.0).
         */
        void setOpacity(float opacity);

        /**
         * @brief Set the size of the sprite.
         * @param w Width of the sprite.
         * @param h Height of the sprite.
         */
        void setSize(float w, float h);

        /**
         * @brief Set the size of the sprite.
         * @param size A `glm::vec2` representing the width and height.
         */
        void setSize(const glm::vec2& size);

        /**
         * @brief Set the pivot point of the sprite.
         * @param x X coordinate of the pivot.
         * @param y Y coordinate of the pivot.
         */
        void setPivot(float x, float y);

        /**
         * @brief Set the pivot point of the sprite.
         * @param pivot A `glm::vec2` representing the pivot coordinates.
         */
        void setPivot(const glm::vec2& pivot);

        /**
         * @brief Set the texture of the sprite.
         * @param texture Pointer to the `Texture2D` object.
         */
        void setTexture(Texture2D* texture);

        /**
         * @brief Get the texture of the sprite.
         * @return Pointer to the `Texture2D` object.
         */
        Texture2D* getTexture();

    private:
        /**
         * @brief Renders the sprite.
         */
        void render() override;

        glm::vec3 _color;       ///< RGB tint of the sprite
        float _opacity;         ///< Alpha value of the sprite
        glm::vec2 _size;        ///< Size (width, height) of the sprite
        glm::vec2 _pivot;       ///< Pivot point (x, y) of the sprite
        std::string texturePath;
        Mesh* _mesh;            ///< Mesh representing the sprite
        Texture2D* _texture;    ///< Texture resource for the sprite
    };
}
