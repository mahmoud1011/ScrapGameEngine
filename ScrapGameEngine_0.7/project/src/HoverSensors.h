#pragma once
#include "Signal.h"
#include "Input.h"
#include "Camera.h"
#include <glad/glad.h>

/**
 * @brief Represents a sensor for detecting hover events.
 *
 * The `HoverSensor` class provides functionality for detecting hover events
 * based on mouse input within a specified area on the screen.
 */
class HoverSensor
{
private:
    float positionX, positionY;       ///< The x and y coordinates of the sensor's position.
    float width, height;              ///< The width and height of the sensor.
    float halfWidth, halfHeight;      ///< The half-width and half-height of the sensor.
    float boxOpacity;                  ///< The opacity of the sensor's bounding box.
    bool isHovered;                    ///< Indicates if the mouse is currently hovering over the sensor.
    bool wasHoveredPreviously;        ///< Indicates if the mouse was previously hovering over the sensor.
    glm::vec4& buttonColor;           ///< Reference to the color of the sensor when not hovered.
    glm::vec4& hoverColor;            ///< Reference to the color of the sensor when hovered.

public:
    /**
     * @brief Signal emitted when the hover event occurs.
     *
     * This signal is emitted when the hover event associated with the `HoverSensor` occurs.
     * Other parts of the program can connect to this signal to perform specific actions
     * when the hover event happens.
     */
    Signal<> onHover;

    /**
     * @brief Constructs a `HoverSensor` with the specified parameters.
     *
     * @param x The x-coordinate of the sensor's position.
     * @param y The y-coordinate of the sensor's position.
     * @param width The width of the sensor.
     * @param height The height of the sensor.
     * @param opacity The opacity of the sensor's box.
     * @param color The color of the sensor when not hovered.
     * @param hoverColor The color of the sensor when hovered.
     */
    HoverSensor(float x, float y, float width, float height, float opacity, glm::vec4& color, glm::vec4& hoverColor)
        : positionX(x), positionY(y), width(width), height(height), boxOpacity(opacity),
        isHovered(false), wasHoveredPreviously(false), buttonColor(color), hoverColor(hoverColor)
    {
        // Clamp minimum dimensions to avoid rendering issues
        this->width = std::max(width, 0.1f);
        this->height = std::max(height, 0.1f);

        halfWidth = this->width * 0.5f;
        halfHeight = this->height * 0.5f;
    }

    /**
     * @brief Updates the state of the `HoverSensor` based on mouse input.
     *
     * This function updates the state of the `HoverSensor` based on the current mouse position and input events.
     */
    void update()
    {
        glm::vec2 mouseScreenPosition = ScrapGameEngine::Input::getMousePosition();
        glm::vec2 mouseWorldPosition = ScrapGameEngine::Camera::screenToWorld(mouseScreenPosition);

        // Determine if the mouse is currently hovering over the sensor
        isHovered = (mouseWorldPosition.x > positionX - halfWidth && mouseWorldPosition.x <= positionX + halfWidth) &&
            (mouseWorldPosition.y > positionY - halfHeight && mouseWorldPosition.y <= positionY + halfHeight);

        if (isHovered)
        {
            if (!wasHoveredPreviously)
            {
                onHover.emit(); // Emit the hover signal if not previously hovered
                wasHoveredPreviously = true;
            }
        }
        else
        {
            wasHoveredPreviously = false; // Reset the hover state
        }
    }

    /**
     * @brief Renders the `HoverSensor`.
     *
     * This function renders the `HoverSensor` as a colored rectangle based on its current state.
     */
    void render()
    {
        glPushMatrix();
        glTranslatef(positionX, positionY, 0);

        // Set color based on hover state
        if (isHovered)
        {
            glColor4f(hoverColor.r, hoverColor.g, hoverColor.b, boxOpacity);
        }
        else
        {
            glColor4f(buttonColor.r, buttonColor.g, buttonColor.b, boxOpacity);
        }

        // Draw the sensor as a rectangle
        glBegin(GL_QUADS);
        glVertex3f(-halfWidth, +halfHeight, 0);
        glVertex3f(-halfWidth, -halfHeight, 0);
        glVertex3f(+halfWidth, -halfHeight, 0);
        glVertex3f(+halfWidth, +halfHeight, 0);
        glEnd();
        glPopMatrix();
    }
};
