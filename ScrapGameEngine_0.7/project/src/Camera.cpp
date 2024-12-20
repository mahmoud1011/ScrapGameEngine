#include "Camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Define static Camera variables
glm::mat4 ScrapGameEngine::Camera::projection = glm::mat4(1.0f);  // Identity matrix as default
glm::mat4 ScrapGameEngine::Camera::view = glm::mat4(1.0f);        // Identity matrix as default
glm::mat4 ScrapGameEngine::Camera::vp = glm::mat4(1.0f);          // Identity matrix as default
glm::vec3 ScrapGameEngine::Camera::position = glm::vec3(0.0f);    // Default position is origin
float ScrapGameEngine::Camera::aspectRatio = 1.0f;                // Default aspect ratio is 1.0
ScrapGameEngine::CameraConfig ScrapGameEngine::Camera::config = {};  // Default configuration
bool ScrapGameEngine::Camera::isDirty = true;                     // Initially set to true

void ScrapGameEngine::Camera::init(CameraConfig cfg, int width, int height)
{
	Camera::config = cfg;
	Camera::recalculate(width, height);
}

void ScrapGameEngine::Camera::recalculate(int width, int height)
{
	Camera::aspectRatio = width / (float)height;

	// Use orthoSize to modify maxY based on the config
	float maxY = Camera::config.orthoSize;
	float maxX = maxY * Camera::aspectRatio;
	float minZ = -1.0f;
	float maxZ = 1.0f;

	// Set the orthographic projection matrix with custom orthoSize
	Camera::projection = glm::ortho(-maxX, maxX, -maxY, maxY, minZ, maxZ);
	Camera::isDirty = true;  // Projection matrix changed, mark as dirty
}

float ScrapGameEngine::Camera::getAspectRatio()
{
	return Camera::aspectRatio;
}

// Get current orthographic size
float ScrapGameEngine::Camera::getOrthoSize() const
{
	return Camera::config.orthoSize;
}

glm::mat4 ScrapGameEngine::Camera::getMatrix_projection()
{
	return Camera::projection;
}

glm::mat4 ScrapGameEngine::Camera::getMatrix_view()
{
	// If the view matrix is dirty, recalculate it
	if (Camera::isDirty)
	{
		// Recalculate the view matrix by translating by the negative position (inverse translation)
		Camera::view = glm::inverse(glm::translate(glm::mat4(1.0f), Camera::position));

		// Recalculate view-projection (vp) matrix
		Camera::vp = Camera::projection * Camera::view;

		// Reset the dirty flag
		Camera::isDirty = false;
	}

	return Camera::view;
}

glm::mat4 ScrapGameEngine::Camera::getMatrix_viewProjection()
{
	// If the view matrix is dirty, recalculate it
	if (Camera::isDirty)
	{
		// Recalculate the view matrix by translating by the negative position
		Camera::view = glm::inverse(glm::translate(glm::mat4(1.0f), Camera::position));

		// Recalculate view-projection matrix (projection * view)
		Camera::vp = Camera::projection * Camera::view;

		// Reset the dirty flag
		Camera::isDirty = false;
	}

	return Camera::vp;
}

void ScrapGameEngine::Camera::translate(glm::vec3 translation)
{
	Camera::position += translation;

	// Mark the matrix as dirty to force recalculation
	Camera::isDirty = true;
}

void ScrapGameEngine::Camera::translate(float x, float y, float z)
{
	Camera::position += glm::vec3(x, y, z);
	Camera::isDirty = true;
}

void ScrapGameEngine::Camera::setPosition(glm::vec3 position)
{
	// Set the camera's position
	Camera::position = position;
	Camera::isDirty = true;
}

void ScrapGameEngine::Camera::setPosition(float x, float y, float z)
{
	// Set the camera's position using float parameters
	Camera::position = glm::vec3(x, y, z);
	Camera::isDirty = true;
}

glm::vec3 ScrapGameEngine::Camera::screenToWorld(glm::vec2 screenPos)
{
	// TODO:: Get screen width and height 
	int screenWidth = 600; // update this with the actual screen width
	int screenHeight = 600; // update this with the actual screen height

	// Normalize screenPos to the -1 to 1 range based on screen width and height
	float normX = 2.0f * (screenPos.x / (float)screenWidth) - 1.0f;
	float normY = 2.0f * (screenPos.y / (float)screenHeight) - 1.0f;

	// Flip Y-axis because screen coordinates typically have the origin at the top-left corner
	normY = -normY;

	// Create a homogeneous vector with z = 0 for 2D projection (z = 0, w = 1)
	glm::vec4 ndcPos(normX, normY, 0.0f, 1.0f);

	// Inverse of the view-projection matrix
	glm::mat4 vpInv = glm::inverse(vp);

	// Convert NDC to world space
	glm::vec4 worldPos = vpInv * ndcPos;

	// Return the x, y coordinates in world space, and set z to 0
	return glm::vec3(worldPos.x, worldPos.y, 0.0f);
}
