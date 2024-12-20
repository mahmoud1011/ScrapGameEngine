#include <iostream>
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Button.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <cmath>
#include "TweenComponent.h"

namespace ScrapGameEngine
{
    TweenComponent::TweenComponent(GameObject* owner)
        : BaseComponent(owner), startValue(0.0f), endValue(0.0f), currentValue(0.0f), duration(0.0f), elapsedTime(0.0f), easingFunction(nullptr), onComplete(nullptr), playing(false), paused(false), type(TweenType::POSITION)
    {
    }

    TweenComponent::~TweenComponent()
    {
    }

    // Function to get an easing function based on the selected easing type
    static std::function<float(float)> getEasingFunction(EasingType easingType)
    {
        switch (easingType)
        {
        case EasingType::LINEAR:
            return [](float t) { return t; };
        case EasingType::EASE_IN:
            return [](float t) { return t * t; };
        case EasingType::EASE_OUT:
            return [](float t) { return t * (2.0f - t); };
        case EasingType::EASE_IN_OUT:
            return [](float t) { return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t; };
        case EasingType::EASE_IN_BACK:
            return [](float t) { return t * t * (3.0f - 2.0f * t); }; // Overshooting ease in
        case EasingType::EASE_OUT_BACK:
            return [](float t) { return (--t) * t * (3.0f + 2.0f * t) + 1.0f; }; // Overshooting ease out
        case EasingType::EASE_IN_ELASTIC:
            return [](float t) {
                float p = 0.3f;
                float s = p / 4.0f;
                return -pow(2.0f, 10.0f * (--t)) * sin((t - s) * (2.0f * M_PI) / p);
                }; // Spring-like ease in
        case EasingType::EASE_OUT_ELASTIC:
            return [](float t) {
                float p = 0.3f;
                float s = p / 4.0f;
                return pow(2.0f, -10.0f * t) * sin((t - s) * (2.0f * M_PI) / p) + 1.0f;
                }; // Spring-like ease out
        case EasingType::EASE_IN_OUT_BOUNCE:
            return [](float t) {
                if (t < 0.5f) {
                    return (1.0f - pow(1.0f - 2.0f * t, 2.0f)) * 0.5f;
                }
                else {
                    return (pow(2.0f * t - 1.0f, 2.0f)) * 0.5f + 0.5f;
                }
                }; // Bouncing ease in/out
        default:
            return [](float t) { return t; };
        }
    }

    void TweenComponent::startTween(TweenType type, const glm::vec3& target, float duration, EasingType easingType, std::function<void()> onComplete)
    {
        this->type = type;
        this->startValue = currentValue;   // Set the start value to the current state.
        this->endValue = target;           // Set the end value to the target state.
        this->duration = duration;         // Set the duration of the tween.
        this->easingType = easingType;     // Set the easing type.
        this->easingFunction = getEasingFunction(easingType); // Get the appropriate easing function.
        this->onComplete = onComplete;     // Set the completion callback.
        this->elapsedTime = 0.0f;          // Reset the elapsed time.
        this->playing = true;              // Start playing the tween.
        this->paused = false;              // Reset pause state.
    }

    void TweenComponent::pause()
    {
        if (playing)
        {
            paused = true;
        }
    }

    void TweenComponent::resume()
    {
        if (paused)
        {
            paused = false;
        }
    }

    void TweenComponent::stop()
    {
        playing = false;
        paused = false;
        onComplete = nullptr; // Clear the completion callback.
    }

    void TweenComponent::update(float deltaTime)
    {
        if (!playing || paused)
        {
            return;
        }

        elapsedTime += deltaTime;

        if (elapsedTime >= duration)
        {
            currentValue = endValue; // Snap to the final value.
            playing = false;

            // Invoke the completion callback if it exists.
            if (onComplete)
            {
                onComplete();
            }
        }
        else
        {
            float t = elapsedTime / duration;   // Progress as a fraction of time.
            t = easingFunction(t);              // Apply easing function.

            switch (type)
            {
            case TweenType::POSITION:
                currentValue = glm::mix(startValue, endValue, t); // Interpolate position.
                gameObject->transform->setPosition(currentValue);
                break;

            case TweenType::SCALE:
                currentValue = glm::mix(startValue, endValue, t); // Interpolate scale.
                gameObject->transform->setScale(currentValue);
                break;

            case TweenType::ROTATION:
                // Interpolate each rotation angle individually.
                glm::vec3 currentRotation{};
                currentRotation.x = glm::mix(glm::radians(startValue.x), glm::radians(endValue.x), t);
                currentRotation.y = glm::mix(glm::radians(startValue.y), glm::radians(endValue.y), t);
                currentRotation.z = glm::mix(glm::radians(startValue.z), glm::radians(endValue.z), t);

                // Convert back to degrees if needed.
                currentValue = glm::degrees(currentRotation);

                // Assuming the setRotation function takes a float y-axis rotation.
                gameObject->transform->setRotation(currentValue.y);
                break;

            case TweenType::FADE:
                currentValue = glm::mix(glm::vec3(startValue.x, startValue.y, 0.0f), glm::vec3(endValue.x, endValue.y, 1.0f), t); // Interpolate for fade effect.
                break;

            case TweenType::COLOR:
                currentValue = glm::mix(startValue, endValue, t); // Interpolate color.

                auto spriteRenderer = gameObject->getComponent<SpriteRenderer>();
                auto button = gameObject->getComponent<Button>();
                if (spriteRenderer)
                {
                    spriteRenderer->setColour(currentValue);
                }
                else if (button)
				{
                    button->setColor(glm::vec4(currentValue, 1.0f)); 
				}

                break;
            }
        }
    }

    bool TweenComponent::isPlaying() const
    {
        return playing;
    }
}
