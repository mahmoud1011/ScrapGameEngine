#pragma once

#include "BaseComponent.h"
#include <glm/glm.hpp>
#include <functional>
#include <vector>

namespace ScrapGameEngine
{
    /**
     * @enum TweenType
     * @brief Enum for different types of tweens.
     */
    enum class TweenType
    {
        POSITION,  ///< Tween affecting the position of a GameObject.
        SCALE,     ///< Tween affecting the scale of a GameObject.
        ROTATION,  ///< Tween affecting the rotation of a GameObject.
        FADE,      ///< Tween affecting the opacity (fade in/out).
        COLOR      ///< Tween affecting the color.
    };

    /**
     * @enum EasingType
     * @brief Enum for different easing types used in tweens.
     */
    enum class EasingType
    {
        NONE,                   ///< No easing.
        LINEAR,                 ///< Linear easing.
        EASE_IN,                ///< Ease in.
        EASE_OUT,               ///< Ease out.
        EASE_IN_OUT,            ///< Ease in and out.
        EASE_IN_BACK,           ///< Ease in with overshoot (back).
        EASE_OUT_BACK,          ///< Ease out with overshoot (back).
        EASE_IN_ELASTIC,        ///< Ease in with elastic effect.
        EASE_OUT_ELASTIC,       ///< Ease out with elastic effect.
        EASE_IN_OUT_BOUNCE      ///< Ease in and out with bounce effect.
    };

    /**
     * @class TweenComponent
     * @brief Component that provides tweening functionality for game objects.
     *
     * This component can be used to animate properties of a GameObject such as position, scale, rotation,
     * opacity, and color. It supports various easing types and allows for pausing, resuming, and stopping
     * tweens.
     */
    class TweenComponent : public BaseComponent
    {
    public:
        /**
         * @brief Constructor for TweenComponent.
         * @param owner Pointer to the GameObject that owns this component.
         */
        TweenComponent(GameObject* owner);

        /**
         * @brief Destructor for TweenComponent.
         */
        ~TweenComponent();

        /**
         * @brief Start a tween with specified parameters.
         * @param type The type of tween to start.
         * @param target The target value for the tween.
         * @param duration The duration of the tween.
         * @param easingType The type of easing to apply.
         * @param onComplete Optional callback function to invoke when the tween completes.
         */
        void startTween(TweenType type, const glm::vec3& target, float duration, EasingType easingType = EasingType::LINEAR, std::function<void()> onComplete = nullptr);

        /**
         * @brief Pause the current tween.
         */
        void pause();

        /**
         * @brief Resume the current tween if it was paused.
         */
        void resume();

        /**
         * @brief Stop the current tween.
         */
        void stop();

        /**
         * @brief Update the tween. Should be called each frame.
         * @param deltaTime The time elapsed since the last frame.
         */
        void update(float deltaTime);

        /**
         * @brief Check if a tween is currently playing.
         * @return True if a tween is currently playing, false otherwise.
         */
        bool isPlaying() const;

    private:
        glm::vec3 startValue;          ///< The starting value of the tween.
        glm::vec3 endValue;            ///< The target (end) value of the tween.
        glm::vec3 currentValue;        ///< The current value of the tween during interpolation.

        float duration;                 ///< The total duration of the tween.
        float elapsedTime;              ///< The time elapsed since the tween started.

        TweenType type;                 ///< The type of tween (e.g., position, scale, rotation).
        EasingType easingType;          ///< The type of easing to apply to the tween.

        std::function<float(float)> easingFunction; ///< The easing function used for interpolation.
        std::function<void()> onComplete;          ///< Optional callback to invoke when the tween completes.

        bool playing;                   ///< Is the tween currently playing.
        bool paused;                    ///< Is the tween currently paused.
    };
}
