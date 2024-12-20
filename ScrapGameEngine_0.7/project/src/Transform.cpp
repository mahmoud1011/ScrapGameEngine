#include "Transform.h"
#include "GameObject.h"

using namespace ScrapGameEngine;

Transform::Transform(GameObject* owner)
    : BaseComponent(owner), localPosition(0.0f), localRotation(0.0f), localScale(1.0f)
{
}

glm::vec2 Transform::getWorldPosition() const
{
    return calculateWorldPosition();
}

float Transform::getWorldRotation() const
{
    return calculateWorldRotation();
}

glm::vec2 ScrapGameEngine::Transform::getPosition() const
{
    return localPosition;
}

float ScrapGameEngine::Transform::getLocalRotation() const
{
    return localRotation;
}

glm::vec2 ScrapGameEngine::Transform::getLocalScale() const
{
    return localScale;
}

void Transform::setPosition(const glm::vec2& position)
{
    localPosition = position;
}

void Transform::setRotation(float rotation)
{
    localRotation = rotation;
}

void ScrapGameEngine::Transform::setScale(const glm::vec2& scale)
{
    localScale = scale;
}

void Transform::setParent(Transform* newParent)
{
    // Remove this transform from the current parent's children
    if (parent != nullptr)
    {
        auto& siblings = parent->children;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), this), siblings.end());
    }

    // Set the new parent and add this transform as its child
    parent = newParent;
    if (newParent != nullptr)
    {
        newParent->children.push_back(this);
    }
}

Transform* ScrapGameEngine::Transform::getParent() const
{
    return parent;
}

const std::vector<Transform*>& ScrapGameEngine::Transform::getChildren() const
{
    return children;
}

glm::vec2 Transform::calculateWorldPosition() const
{
    if (parent)
    {
        glm::vec2 parentWorldPos = parent->getWorldPosition();
        float parentRotation = glm::radians(parent->getWorldRotation());

        glm::vec2 rotatedLocalPos = {
            localPosition.x * cos(parentRotation) - localPosition.y * sin(parentRotation),
            localPosition.x * sin(parentRotation) + localPosition.y * cos(parentRotation)
        };

        return parentWorldPos + rotatedLocalPos;
    }
    return localPosition;
}

float Transform::calculateWorldRotation() const
{
    return parent ? parent->getWorldRotation() + localRotation : localRotation;
}

glm::vec2 ScrapGameEngine::Transform::calculateWorldScale() const
{
    if (!parent) return localScale;
    return parent->calculateWorldScale() * localScale;
}
