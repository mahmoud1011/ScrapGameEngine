#include "Time.h"
#include <glfw/glfw3.h>

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

	do
	{
		currentTime = (float)glfwGetTime();
		deltaTime = currentTime - prevTime;
	} while (deltaTime < frameTime);

    prevTime = currentTime;
}

