#include "Input.h"
#include "AppWindow.h"
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

namespace ScrapGameEngine
{
	static GLFWwindow* window_Impl;

	// Unordered_maps for Keycode
	std::unordered_map<KeyCode, bool> Input::key_states_current;
	std::unordered_map<KeyCode, bool> Input::key_states_previous;

	// Unordered_maps for Mouse
	std::unordered_map<MouseButtonCode, bool> Input::mouse_states_current;
	std::unordered_map<MouseButtonCode, bool> Input::mouse_states_previous;

	// define the unordered_maps
	void Input::init(void* procAddress)
	{
		AppWindow* appWindow = static_cast<AppWindow*>(procAddress);
		void* nativeWindow = appWindow->getNativeWindow();
		window_Impl = static_cast<GLFWwindow*>(nativeWindow);

		// set sticky keys and buttons
		glfwSetInputMode(window_Impl, GLFW_STICKY_KEYS, GLFW_TRUE);
		glfwSetInputMode(window_Impl, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

		// init defaults
		// key states
		key_states_current = {
			 { KeyCode::ESCAPE, false },
			 { KeyCode::F1, false },
			 { KeyCode::F2, false },
			 { KeyCode::F3, false },
			 { KeyCode::F4, false },
			 { KeyCode::F5, false },
			 { KeyCode::F6, false },
			 { KeyCode::F7, false },
			 { KeyCode::F8, false },
			 { KeyCode::F9, false },
			 { KeyCode::F10, false },
			 { KeyCode::F11, false },
			 { KeyCode::F12, false },
			 { KeyCode::ALPHA1, false },
			 { KeyCode::ALPHA2, false },
			 { KeyCode::ALPHA3, false },
			 { KeyCode::ALPHA4, false },
			 { KeyCode::ALPHA5, false },
			 { KeyCode::ALPHA6, false },
			 { KeyCode::ALPHA7, false },
			 { KeyCode::ALPHA8, false },
			 { KeyCode::ALPHA9, false },
			 { KeyCode::ALPHA0, false },
			 { KeyCode::A, false },
			 { KeyCode::B, false },
			 { KeyCode::C, false },
			 { KeyCode::D, false },
			 { KeyCode::E, false },
			 { KeyCode::F, false },
			 { KeyCode::G, false },
			 { KeyCode::H, false },
			 { KeyCode::I, false },
			 { KeyCode::J, false },
			 { KeyCode::K, false },
			 { KeyCode::L, false },
			 { KeyCode::M, false },
			 { KeyCode::N, false },
			 { KeyCode::O, false },
			 { KeyCode::P, false },
			 { KeyCode::Q, false },
			 { KeyCode::R, false },
			 { KeyCode::S, false },
			 { KeyCode::T, false },
			 { KeyCode::U, false },
			 { KeyCode::V, false },
			 { KeyCode::W, false },
			 { KeyCode::X, false },
			 { KeyCode::Y, false },
			 { KeyCode::Z, false },
			 { KeyCode::UP, false },
			 { KeyCode::DOWN, false },
			 { KeyCode::RIGHT, false },
			 { KeyCode::LEFT, false },
			 { KeyCode::SHIFT, false },
			 { KeyCode::CONTROL, false },
			 { KeyCode::ALT, false },
			 { KeyCode::SPACE, false },
			 { KeyCode::ENTER, false },
			 { KeyCode::BACKSPACE, false },
			 { KeyCode::TAB, false },
			 { KeyCode::ESC, false },
			 { KeyCode::CAPS_LOCK, false },
			 { KeyCode::PAGE_UP, false },
			 { KeyCode::PAGE_DOWN, false },
			 { KeyCode::HOME, false },
			 { KeyCode::END, false },
			 { KeyCode::INSERT, false },
			 { KeyCode::DELETE, false },
		};

		// mouse states
		mouse_states_current = {
		 { MouseButtonCode::LEFT, false },
		 { MouseButtonCode::RIGHT, false },
		 { MouseButtonCode::MIDDLE, false },
		};

		// In C++, doing this makes a copy of the unordered_map.
		// This works in our favour, reducing boilerplate codes.
		key_states_previous = key_states_current;

		mouse_states_previous = mouse_states_current;
	}

	void Input::process()
	{
		// Previous key states
		for (const auto& pair : key_states_current) {
			key_states_previous[pair.first] = pair.second; // Copy current state to previous
		}

		// Current key states
		for (auto& pair : key_states_current) {
			pair.second = (glfwGetKey(window_Impl, static_cast<int>(pair.first)) == GLFW_PRESS);
		}

		// Previous mouse states
		for (const auto& pair : mouse_states_current) {
			mouse_states_previous[pair.first] = pair.second;
		}

		// Current mouse states
		for (auto& pair : mouse_states_current) {
			pair.second = (glfwGetMouseButton(window_Impl, static_cast<int>(pair.first)) == GLFW_PRESS);
		}
	}

	// Keyboard ===========================================================
	bool Input::getKey(const KeyCode keyCode)
	{
		// Find the key in key_states_current
		auto it = key_states_current.find(keyCode);

		if (it != key_states_current.end()) {
			// Key found, return its current state
			return it->second;
		}
		else {
			// Key not found, return false
			return false;
		}
	}

	bool Input::getKeyDown(const KeyCode keyCode)
	{
		// Find the key in key_states_current
		auto it = key_states_current.find(keyCode);

		if (it != key_states_current.end())
		{
			bool curValue = it->second; // Current state
			bool oldValue = key_states_previous[keyCode]; // Previous state

			// Return true if the key is currently pressed and was not pressed before
			return curValue && !oldValue;
		}
		// Key not found, return false
		return false;
	}

	bool Input::getKeyUp(const KeyCode keyCode)
	{
		// Find the key in key_states_current
		auto it = key_states_current.find(keyCode);

		if (it != key_states_current.end())
		{
			bool curValue = it->second; // Current state
			bool oldValue = key_states_previous[keyCode]; // Previous state

			return !curValue && oldValue;
		}
		// Key not found, return false
		return false;
	}
	//===================================================================

	// Get mouse position relative to the top-left of the window
	glm::vec2 Input::getMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(window_Impl, &xpos, &ypos);
		return glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
	}

	// Mouse ===========================================================
	bool Input::getMouseButton(const MouseButtonCode keyCode)
	{
		auto it = mouse_states_current.find(keyCode);

		if (it != mouse_states_current.end())
		{
			// Key found, return its current state
			return it->second;
		}
		else {
			// Key not found, return false
			return false;
		}
	}

	bool Input::getMouseButtonDown(const MouseButtonCode keyCode)
	{
		auto it = mouse_states_current.find(keyCode);

		if (it != mouse_states_current.end())
		{
			bool curValue = it->second; // Current state
			bool oldValue = mouse_states_previous[keyCode]; // Previous state

			return curValue && !oldValue;
		}

		// Key not found, return false
		return false;
	}

	bool Input::getMouseButtonUp(const MouseButtonCode keyCode)
	{
		auto it = mouse_states_current.find(keyCode);

		if (it != mouse_states_current.end())
		{
			bool curValue = it->second; // Current state
			bool oldValue = mouse_states_previous[keyCode]; // Previous state

			return !curValue && oldValue;
		}

		// Key not found, return false
		return false;
	}
	//===================================================================
}