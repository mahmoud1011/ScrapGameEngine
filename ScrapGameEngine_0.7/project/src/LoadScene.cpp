#include "Scheduler.h"
#include "TweenComponent.h"
#include "TextureAllocator.h"
#include "SceneStateMachine.h"
#include "SpriteRenderer.h"
#include "Application.h"
#include "LoadScene.h"

using namespace ScrapGameEngine;

std::unique_ptr<GameObject> LoadScene::gameObject = nullptr;

void LoadScene::onInitialize()
{
    // Initialize the loading bar UI
    gameObject = std::make_unique<GameObject>();
    gameObject->setName("Loading GameObject");
    GameObject* bgObject = GameObject::Create("BG");
    if (!bgObject)
    {
        std::cerr << "Failed to create bg GameObject" << std::endl;
    }

    // Configure the texture
    TextureConfig cfg{};
    cfg.wrapModeX = TextureWrapMode::REPEAT;
    cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
    cfg.filterMode = TextureFilterMode::NEAREST;
    cfg.borderColor = { 1.0f, 0.0f, 0.0f, 1.0f };
    cfg.generateMipmaps = false;
    
    // Load a texture using allocator
    std::string texturePath = "../assets/Assets/LoadBar.png";
    auto loadingTexture = TextureAllocator::getTexture(texturePath, cfg);

    // Load a texture using allocator
    std::string texturePath1 = "../assets/Assets/LoadBG.png";
    auto loadingTexture1 = TextureAllocator::getTexture(texturePath1, cfg);

    auto* loadingSprite = gameObject->addComponent<SpriteRenderer>();
    auto* bgSprite = bgObject->addComponent<SpriteRenderer>();
    auto* tween = gameObject->addComponent<TweenComponent>();
    auto& window = Application::getInstance()->getWindow();

    loadingSprite->awake();
    loadingSprite->setTexture(loadingTexture);
    loadingSprite->setSize(1.25, 1);

	bgSprite->awake();
	bgSprite->setTexture(loadingTexture1); 
	bgSprite->setSize({ window.getScreenSize().x, window.getScreenSize().y}); 

    tween->awake();

    _progress = 0.0f;

    setLoadTime(3.0f);
    load("MainMenuScene");
}

void LoadScene::onUpdate(float deltaTime)
{
    time += deltaTime;

    if (gameObject)
	{
		gameObject->runComponentUpdate(deltaTime);

        auto* tween = gameObject->getComponent<TweenComponent>();
        if (tween)
        {
            tween->update(deltaTime);
        }
	}

    // Update progress
    if (_progress > 0.0f)
    {
        _progress -= deltaTime;
    }
    else if (_progress <= 0.0f && _sceneName != "") // Ensure the scene loads at the end of the loading time
    {
        SceneStateMachine::loadScene(_sceneName); // Load the scene when the load time is up
    }

    // Update loading bar
    updateLoadingBar();
}

void LoadScene::onRender()
{
    if (gameObject)
    {
        gameObject->runComponentRender();
    }
}

void LoadScene::onDeactivate()
{
    if (gameObject)    
        gameObject->destroy();
}

void LoadScene::load(const std::string& sceneName)
{
    _sceneName = sceneName;
}

void LoadScene::setLoadTime(float time)
{
    _loadTime = time;
    _progress = _loadTime;
}

void LoadScene::updateLoadingBar() const
{
    auto tween = gameObject->getComponent<TweenComponent>();
    if (tween) {
        // Define the rotation speed in degrees per second.
        float rotationSpeed = 0.025f; // Faster rotation speed
        float rotationAmount = _progress / _loadTime * 1080.0f;
        tween->startTween(ScrapGameEngine::TweenType::ROTATION, { 0.0f, rotationAmount, 0.0f }, rotationSpeed, ScrapGameEngine::EasingType::EASE_IN_OUT_BOUNCE);
    }
}
