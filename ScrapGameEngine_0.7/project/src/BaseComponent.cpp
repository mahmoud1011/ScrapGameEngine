#include "BaseComponent.h"

namespace ScrapGameEngine
{
	BaseComponent::BaseComponent(GameObject* go) : gameObject(go) {}
	BaseComponent::~BaseComponent() {} // Required for subclasses to work properly.

	void BaseComponent::awake()
	{ }

	void BaseComponent::start()
	{ }

	void BaseComponent::update(float deltaTime)
	{ }

	void BaseComponent::render()
	{ }

	void BaseComponent::destroy()
	{
		flaggedForDeletion = true;
	}

	bool BaseComponent::shouldDestroy() const
	{
		return flaggedForDeletion;
	}
}
