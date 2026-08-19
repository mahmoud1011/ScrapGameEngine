#include "core/Time.h"
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>

static float currentTime = 0.0f;
static float prevTime = 0.0f;
static float deltaTime = 0.0f;

float ScrapGameEngine::Time::getTime()
{
	return currentTime;
}

float ScrapGameEngine::Time::getDeltaTime()
{
	return deltaTime;
}

void ScrapGameEngine::Time::processTime(float frameTime)
{
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - prevTime;

	// Frame limiting. The previous implementation spun on glfwGetTime(), which held a
	// CPU core at 100% for the whole wait. Sleep for the bulk of the remaining time and
	// spin only for the last fraction, since sleep resolution is coarse (and varies by
	// platform) but the final approach needs to be accurate.
	if (frameTime > 0.0f && deltaTime < frameTime)
	{
		constexpr float spinMargin = 0.002f; // 2ms, comfortably above typical sleep jitter

		const float sleepFor = (frameTime - deltaTime) - spinMargin;
		if (sleepFor > 0.0f)
		{
			std::this_thread::sleep_for(
				std::chrono::duration<float>(sleepFor));
		}

		do
		{
			std::this_thread::yield();
			currentTime = (float)glfwGetTime();
			deltaTime = currentTime - prevTime;
		} while (deltaTime < frameTime);
	}

	prevTime = currentTime;
}
