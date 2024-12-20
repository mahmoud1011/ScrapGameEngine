#include "BaseComponent.h"
#include <iostream>
#include <string>
#include "Signal.h"
#include <glm/glm.hpp>

namespace ScrapGameEngine
{
    /**
     * @class Button
     * @brief Represents a UI button component for handling user interactions.
     *
     * This class handles the rendering, clicking, releasing, and hover interactions
     * of a button. It uses signals to notify when the button is clicked or released,
     * and provides functionalities to set size, color, and transparency.
     */
    class Button : public BaseComponent
    {
    public:
        /**
         * @brief Constructs a Button instance attached to the given GameObject.
         * @param owner The GameObject to which this Button component is attached.
         */
        Button(GameObject* owner);

        // Signal to notify when the button is clicked
        Signal<> onClick;

        // Signal to notify when the button is released
        Signal<> onRelease;

        /**
         * @brief Renders the button on the screen.
         */
        void render() override;

        /**
         * @brief Triggers when the mouse cursor enters the button area.
         */
        void OnCursorEntered();

        /**
         * @brief Sets the size of the button.
         * @param w Width of the button.
         * @param h Height of the button.
         */
        void setSize(int w, int h);

        /**
         * @brief Sets the size of the button.
         * @param size A vector representing the size of the button.
         */
        void setSize(const glm::vec2& size);

        /**
         * @brief Sets the transparency of the button.
         * @param boxTransparency The transparency value (0.0 to 1.0).
         */
        void setTransparency(float boxTransparency);

        /**
         * @brief Sets the color of the button.
         * @param color The color as a glm::vec4 (RGBA).
         */
        void setColor(const glm::vec4& color);

        /**
         * @brief Sets the color of the button.
         * @param color The color as a glm::vec4 (RGBA).
         */
        void setColour(const glm::vec4& color);

        /**
         * @brief Sets the hover color of the button.
         * @param color The hover color as a glm::vec4 (RGBA).
         */
        void setHoverColor(const glm::vec4& color);

        /**
         * @brief Sets the hover color of the button.
         * @param color The hover color as a glm::vec4 (RGBA).
         */
        void setHoverColour(const glm::vec4& color);

    private:
        glm::vec2 _size;       ///< The size of the button (width, height).
        glm::vec4 _color;      ///< The color of the button.
        glm::vec4 _hoverColor; ///< The color of the button when hovered over.

        int _x, _y;            ///< The x and y coordinates of the button.
        float boxOpacity;      ///< The opacity of the button background.

        bool _isPressed;       ///< Indicates if the button is currently pressed.

        /**
         * @brief Checks if the mouse is over the button.
         * @param mouseX The x-coordinate of the mouse.
         * @param mouseY The y-coordinate of the mouse.
         * @return True if the mouse is over the button, false otherwise.
         */
        bool isMouseOver(int mouseX, int mouseY);
    };
}
