#include "TextureAllocator.h"
#include "GameObject.h"
#include "Button.h"
#include "HoverSensors.h"
#include "Input.h"

ScrapGameEngine::Button::Button(GameObject* owner)
    : BaseComponent(owner), _size(1, 1), boxOpacity(1.0f), _isPressed(false), _color(glm::vec4(1.0f)), _hoverColor(glm::vec4(0.5f)) 
{
}

void ScrapGameEngine::Button::render()
{
    if (!gameObject || !gameObject->transform) return;

    auto pos = gameObject->transform->getPosition();
    HoverSensor sensor(pos.x, pos.y, _size.x, _size.y, boxOpacity, _color, _hoverColor);

    // Connect hover signal to OnCursorEntered
    int id1 = sensor.onHover.connect([this]() { this->OnCursorEntered(); });

    sensor.update();
    sensor.render();
}

void ScrapGameEngine::Button::OnCursorEntered()
{

    if (Input::getMouseButtonDown(MouseButtonCode::LEFT))
    {
        onClick.emit();
        _isPressed = true;
    }
    else if (Input::getMouseButtonUp(MouseButtonCode::LEFT))
    {
        onRelease.emit();
        _isPressed = false;
    }
}

void ScrapGameEngine::Button::setSize(int w, int h)
{
    _size = glm::vec2(w, h);
}

void ScrapGameEngine::Button::setSize(const glm::vec2& size)
{
    _size = size;
}

void ScrapGameEngine::Button::setTransparency(float boxTransparency)
{
    boxOpacity = glm::clamp(boxTransparency, 0.0f, 1.0f);
}

void ScrapGameEngine::Button::setColor(const glm::vec4& color)
{
    _color = color;
}

void ScrapGameEngine::Button::setColour(const glm::vec4& color)
{
    _color = color;
}

void ScrapGameEngine::Button::setHoverColor(const glm::vec4& color)
{
    _hoverColor = color;
}

void ScrapGameEngine::Button::setHoverColour(const glm::vec4& color)
{
    _hoverColor = color;
}

bool ScrapGameEngine::Button::isMouseOver(int mouseX, int mouseY)
{
    if (!gameObject || !gameObject->transform) return false;

    auto pos = gameObject->transform->getPosition();
    return mouseX >= pos.x && mouseX <= (pos.x + _size.x) &&
        mouseY >= pos.y && mouseY <= (pos.y + _size.y);
}
