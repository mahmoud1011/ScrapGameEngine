#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

namespace ScrapGameEngine
{
    /**
     * @enum KeyCode
     * @brief Enum class representing different key codes for keyboard input.
     *
     * This enum includes a variety of key codes for both standard keys and special keys like function keys, numeric keys, letter keys,
     * arrow keys, modifier keys, and other keys such as space, enter, backspace, etc.
     */
    enum class KeyCode
    {
        // Escape and Function Keys
        ESCAPE = 256,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,

        // Numeric Keys
        ALPHA1 = 49,
        ALPHA2 = 50,
        ALPHA3 = 51,
        ALPHA4 = 52,
        ALPHA5 = 53,
        ALPHA6 = 54,
        ALPHA7 = 55,
        ALPHA8 = 56,
        ALPHA9 = 57,
        ALPHA0 = 48,

        // Numeric Keypad
        NUMPAD_0 = 320,
        NUMPAD_1 = 321,
        NUMPAD_2 = 322,
        NUMPAD_3 = 323,
        NUMPAD_4 = 324,
        NUMPAD_5 = 325,
        NUMPAD_6 = 326,
        NUMPAD_7 = 327,
        NUMPAD_8 = 328,
        NUMPAD_9 = 329,

        // Letter Keys
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,

        // Arrow Keys
        UP = 265,
        DOWN = 264,
        LEFT = 263,
        RIGHT = 262,

        // Modifier Keys
        SHIFT = 340,
        CONTROL = 341,
        ALT = 342,

        // Other Keys
        SPACE = 32,
        ENTER = 257,
        BACKSPACE = 259,
        TAB = 258,
        ESC = 256,
        CAPS_LOCK = 280,
        PAGE_UP = 266,
        PAGE_DOWN = 267,
        HOME = 268,
        END = 269,
        INSERT = 260,
        DELETE = 261
    };

    /**
     * @enum MouseButtonCode
     * @brief Enum class representing mouse button codes.
     *
     * This enum includes codes for the left, right, and middle mouse buttons.
     */
    enum class MouseButtonCode
    {
        LEFT = 0,
        RIGHT = 1,
        MIDDLE = 2
    };

    /**
     * @class Input
     * @brief Handles input management for the application, including keyboard and mouse input.
     *
     * The Input class manages the current and previous state of keys and mouse buttons. It provides methods to query the state of keys,
     * mouse buttons, and mouse position. The input states are updated through the `process()` method which needs to be called
     * during the application update cycle.
     */
    class Input
    {
    private:
        friend class Application;
        Input() = delete;

        /**
         * @brief Initialize the Input class.
         * @param procAddress Address of the procedure to initialize input processing.
         */
        static void init(void* procAddress);

        /**
         * @brief Process the input states.
         *
         * This function updates the current state of keys and mouse buttons based on the latest input events.
         */
        static void process();

        static std::unordered_map<KeyCode, bool> key_states_current; ///< Current state of keys.
        static std::unordered_map<KeyCode, bool> key_states_previous; ///< Previous state of keys.

        static std::unordered_map<MouseButtonCode, bool> mouse_states_current; ///< Current state of mouse buttons.
        static std::unordered_map<MouseButtonCode, bool> mouse_states_previous; ///< Previous state of mouse buttons.

    public:
        /**
         * @brief Check if a key is currently pressed.
         * @param keyCode The key code to check.
         * @return True if the key is currently pressed, false otherwise.
         */
        static bool getKey(const KeyCode keyCode);

        /**
         * @brief Check if a key has just been pressed.
         * @param keyCode The key code to check.
         * @return True if the key was just pressed, false otherwise.
         */
        static bool getKeyDown(const KeyCode keyCode);

        /**
         * @brief Check if a key has just been released.
         * @param keyCode The key code to check.
         * @return True if the key was just released, false otherwise.
         */
        static bool getKeyUp(const KeyCode keyCode);

        /**
         * @brief Get the current position of the mouse cursor.
         * @return The current mouse position as a glm::vec2.
         */
        static glm::vec2 getMousePosition();

        /**
         * @brief Check if a mouse button is currently pressed.
         * @param keyCode The mouse button code to check.
         * @return True if the button is currently pressed, false otherwise.
         */
        static bool getMouseButton(const MouseButtonCode keyCode);

        /**
         * @brief Check if a mouse button has just been released.
         * @param keyCode The mouse button code to check.
         * @return True if the button was just released, false otherwise.
         */
        static bool getMouseButtonUp(const MouseButtonCode keyCode);

        /**
         * @brief Check if a mouse button has just been pressed.
         * @param keyCode The mouse button code to check.
         * @return True if the button was just pressed, false otherwise.
         */
        static bool getMouseButtonDown(const MouseButtonCode keyCode);
    };
}
