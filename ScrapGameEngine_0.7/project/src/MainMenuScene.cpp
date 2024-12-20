#include "Graphics.h"
#include "Input.h" 
#include "SceneStateMachine.h"
#include "TextureAllocator.h"
#include "SpriteRenderer.h"
#include "Button.h"
#include "AudioSource.h"
#include <iostream>
#include "Scheduler.h"
#include <memory>
#include "MainMenuScene.h"

using namespace ScrapGameEngine;

std::unique_ptr<GameObject> MainMenuScene::gameObject = nullptr;  

void MainMenuScene::onInitialize()
{
    time = 0.0f;

    // Configure the texture
    TextureConfig cfg{};
    cfg.wrapModeX = TextureWrapMode::CLAMP;
    cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
    cfg.filterMode = TextureFilterMode::LINEAR;
    cfg.borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    cfg.generateMipmaps = false;

    // Load a texture using allocator
    std::string texturePath = "../assets/Assets/MainMenu/Play_Button.png";
    texture = TextureAllocator::getTexture(texturePath, cfg);

    // Initialize the GameObject with a position and rotation
    gameObject = std::make_unique<GameObject>();
    gameObject->setName("Button GameObject");
    gameObject->transform->setPosition({ 0.0f, -0.25f }); 
    gameObject->transform->setRotation(0.0f);           

    auto button = gameObject->addComponent<Button>();
    auto spriteRenderer = gameObject->addComponent<SpriteRenderer>();

    button->setSize(1, 1);
	button->setTransparency(0);
    
    spriteRenderer->awake();
    spriteRenderer->setSize(1, 1);
    spriteRenderer->setTexture(texture);

    // Create an AudioSource instance
    auto audioSource = gameObject->getComponent<AudioSource>();
    if (!audioSource)
    {
        audioSource = gameObject->addComponent<AudioSource>();
        const std::string audioFile = "../assets/Assets/Game/SFX/ButtonPress.mp3";
        audioSource->load(audioFile);
    }

    // Configure the AudioSource properties
    audioSource->setVolume(0.3f);

    // Button onClick callback
    button->onClick.connect([audioSource]()
        {
            // Play the audio if not already playing
            if (!audioSource->isPlaying())
            {
                audioSource->play();
            }

            // Check when the audio is done playing
            Scheduler::addRepeatingTask([audioSource]() -> bool {
                if (!audioSource->isPlaying())
                {
                    SceneStateMachine::loadScene("GameScene");
                    return true;
                }
                return false; 
                });
        });
}

void MainMenuScene::onUpdate(float deltaTime)
{
    time += deltaTime;

    Scheduler::update(deltaTime);

    if (Input::getKey(KeyCode::TAB))
    {
        SceneStateMachine::loadScene("SplashScreenScene");
    }
}

void MainMenuScene::onRender()
{    
    gameObject->runComponentRender();
}

void MainMenuScene::onDeactivate()
{

    if (gameObject)
    {
        auto audioSource = gameObject->getComponent<AudioSource>();
        if (audioSource)
        {
            audioSource->stop(); 
            audioSource->cleanup(); 
        }
        gameObject->destroy();
    }
    
    TextureAllocator::returnTexture(texture);
    TextureAllocator::releaseUnusedTextures();
}
