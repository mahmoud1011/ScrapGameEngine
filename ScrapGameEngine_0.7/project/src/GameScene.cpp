#include "Graphics.h"
#include "Input.h" 
#include "SceneStateMachine.h"
#include "TextureAllocator.h"
#include "SpriteRenderer.h"
#include "Button.h"
#include "TweenComponent.h"
#include "Scheduler.h"
#include <iostream>
#include <memory>
#include "GameScene.h"

std::unique_ptr<GameObject> GameScene::tutorialObject = nullptr;
// Add these declarations to your `GameScene` class
std::vector<std::vector<std::string>> audioSets; // Holds multiple sets of audio files
int currentAudioSetIndex = 0; // Tracks the current audio set

void GameScene::onInitialize()
{
    time = 0.0f;

    initializeAudioSets();

    const int rows = 2; // Number of rows
    const int columns = 3; // Number of columns
    const int buttonCount = rows * columns; // Total number of buttons
    const float spacing = 1.0f; // Spacing between buttons
    glm::vec2 startPosition = { -spacing * (columns - 1) / 2.0f, spacing * (rows - 1) / 2.0f }; // Starting position of the first button

    for (int i = 0; i < buttonCount; ++i)
    {
        // Create a music pad button GameObject
        GameObject* buttonObject = GameObject::Create("MusicPadButton" + std::to_string(i));
        if (!buttonObject) 
        {
            std::cerr << "Failed to create GameObject for button " << i << std::endl;
            continue;
        }
        musicPadButtons.push_back(buttonObject);

        buttonObject->transform->setScale({ 0.0f, 0.0f });

        // Configure the texture
        TextureConfig cfg{};
        cfg.wrapModeX = TextureWrapMode::REPEAT;
        cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
        cfg.filterMode = TextureFilterMode::NEAREST;
        cfg.borderColor = { 1.0f, 0.0f, 0.0f, 1.0f };
        cfg.generateMipmaps = false;

        // Load a texture using allocator
        std::string texturePath = "../assets/Assets/Game/MusicPad" + std::to_string(i) + ".png";
        auto* texture = TextureAllocator::getTexture(texturePath, cfg);
        if (!texture)
        {
            std::cerr << "Failed to load texture: " << texturePath << std::endl;
            continue;
        }

        // Add components
        auto button = buttonObject->addComponent<Button>();
        if (!button) {
            std::cerr << "Failed to add Button component to GameObject " << i << std::endl;
            continue;
        }

        auto sprite = buttonObject->addComponent<SpriteRenderer>();
        if (!sprite) {
            std::cerr << "Failed to add SpriteRenderer component to GameObject " << i << std::endl;
            continue;
        }

        auto audioSource = buttonObject->addComponent<AudioSource>();
        if (!audioSource) {
            std::cerr << "Failed to add AudioSource component to GameObject " << i << std::endl;
            continue;
        }

        auto tweenComponent = buttonObject->addComponent<TweenComponent>();
        if (!tweenComponent)
        {
            std::cerr << "Failed to add TweenComponent component to GameObject " << i << std::endl;
            continue;
        }

        button->setSize(1, 1);
        button->setTransparency(0);

        sprite->awake();
        sprite->setSize(0.3f, 0.3f);
        sprite->setTexture(texture);

        // Load audio for the button from the initial set
        if (i < audioSets[currentAudioSetIndex].size())
        {
            audioSource->load(audioSets[currentAudioSetIndex][i]);
        }

        audioSource->setVolume(0.3f);
        audioSource->setLooping(true);

        // Map the button index to its audio source
        buttonAudioMap[i] = audioSource;

        // Calculate position based on row and column
        int row = i / columns;
        int col = i % columns;
        if (buttonObject->transform)
        {
            buttonObject->transform->setPosition(startPosition + glm::vec2(col * spacing, -row * spacing));
        }
        else
        {
            std::cerr << "GameObject " << i << " does not have a valid transform component." << std::endl;
        }

        // Set up button functionality
        button->onClick.connect([audioSource]() {
            if (tutorialObject) return;

            if (audioSource) audioSource->play(); // Play audio on press
            });

        button->onRelease.connect([audioSource]() {
            if (tutorialObject) return;
            if (audioSource) audioSource->stop(); // Stop audio on release
            });
    }

    if (!tutorialObject)
    {
        // Create and configure the tutorial GameObject
        tutorialObject = std::make_unique<GameObject>();
        tutorialObject->setName("Tutorial GameObject");
        tutorialObject->transform->setPosition({ 0.0f, 0.0f });
        tutorialObject->transform->setRotation(0.0f);

        // Configure the texture
        TextureConfig cfg{};
        cfg.wrapModeX = TextureWrapMode::CLAMP;
        cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
        cfg.filterMode = TextureFilterMode::LINEAR;
        cfg.borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };
        cfg.generateMipmaps = false;

        // Load a texture using allocator
        std::string texturePath = "../assets/Assets/Game/Game_Tutorial.png";
        tutorialtexture = TextureAllocator::getTexture(texturePath, cfg);

        auto tutorialSprite = tutorialObject->addComponent<SpriteRenderer>();

        tutorialSprite->awake();
        tutorialSprite->setSize(1.25f, 1.5f);
        tutorialSprite->setTexture(tutorialtexture);
    }

    clickAudioSource = GameObject::Create("Click AudioSource");
    auto* clickComponent = clickAudioSource->addComponent<AudioSource>();

    clickComponent->load("../assets/Assets/Game/SFX/ButtonPress.mp3");
    clickComponent->setVolume(0.3f);
    clickComponent->setLooping(false);
}

void GameScene::onUpdate(float deltaTime)
{
    time += deltaTime;

    // Update all music pad buttons
    for (auto* buttonObject : musicPadButtons)
    {
        if (buttonObject)
        {
            buttonObject->runComponentUpdate(deltaTime);

            auto* tween = buttonObject->getComponent<TweenComponent>();
            if (tween)
            {
                tween->update(deltaTime);
            }
        }
    }

    if (!tutorialObject)
    {
        // Check for keyboard input
        if (Input::getKeyDown(KeyCode::ALPHA1)) buttonAudioMap[0]->play();
        if (Input::getKeyDown(KeyCode::ALPHA2)) buttonAudioMap[1]->play();
        if (Input::getKeyDown(KeyCode::ALPHA3)) buttonAudioMap[2]->play();
        if (Input::getKeyDown(KeyCode::ALPHA4)) buttonAudioMap[3]->play();
        if (Input::getKeyDown(KeyCode::ALPHA5)) buttonAudioMap[4]->play();
        if (Input::getKeyDown(KeyCode::ALPHA6)) buttonAudioMap[5]->play();

        if (Input::getKeyUp(KeyCode::ALPHA1)) buttonAudioMap[0]->stop();
        if (Input::getKeyUp(KeyCode::ALPHA2)) buttonAudioMap[1]->stop();
        if (Input::getKeyUp(KeyCode::ALPHA3)) buttonAudioMap[2]->stop();
        if (Input::getKeyUp(KeyCode::ALPHA4)) buttonAudioMap[3]->stop();
        if (Input::getKeyUp(KeyCode::ALPHA5)) buttonAudioMap[4]->stop();
        if (Input::getKeyUp(KeyCode::ALPHA6)) buttonAudioMap[5]->stop();
    }
    else
    {
        // Check for mouse input
        if (Input::getMouseButtonDown(MouseButtonCode::RIGHT))
        {
            // Perform necessary cleanup and reset tutorial object
            tutorialObject->destroy();
            tutorialObject = nullptr;

            // Update all music pad buttons with scaling tween
            for (auto* buttonObject : musicPadButtons)
            {
                if (buttonObject)
                {
                    auto* tween = buttonObject->getComponent<TweenComponent>();
                    if (tween)
                    {
                        tween->startTween(ScrapGameEngine::TweenType::SCALE, { 1.0f, 1.0f, 1.0f }, 1.25f, ScrapGameEngine::EasingType::EASE_IN_OUT);
                    }

                    auto* clickComponent = clickAudioSource->getComponent<AudioSource>();
                    if (clickComponent)
                    {
                        clickComponent->play();
                    }
                }
            }
        }
    }

    // Switch audio sets
    if (Input::getKeyDown(KeyCode::RIGHT))
    {
        applyAudioSet(currentAudioSetIndex + 1); // Next set

        auto* clickComponent = clickAudioSource->getComponent<AudioSource>();
        if (clickComponent)
		{
			clickComponent->play();
		}
    }
    if (Input::getKeyDown(KeyCode::LEFT))
    {
        applyAudioSet(currentAudioSetIndex - 1); // Previous set
        
        auto* clickComponent = clickAudioSource->getComponent<AudioSource>();
        if (clickComponent)
        {
            clickComponent->play();
        }
    }

    // Handle GameObject destruction when a certain key is pressed
    if (Input::getKey(KeyCode::TAB))
    {
        SceneStateMachine::loadScene("SplashScreenScene");

        auto* clickComponent = clickAudioSource->getComponent<AudioSource>();
        if (clickComponent)
        {
            clickComponent->play();
        }
    }
}

void GameScene::onRender()
{
    // Render the tutorial GameObject
    if (tutorialObject)
    {
        tutorialObject->runComponentRender();
    }

    // Render all music pad buttons
    for (auto* buttonObject : musicPadButtons)
    {
        if (buttonObject) buttonObject->runComponentRender();
    }
}

void GameScene::onDeactivate()
{
    if (tutorialObject)
    {
        tutorialObject->destroy();
    }

    // Clean up dynamically allocated GameObjects
    for (auto* buttonObject : musicPadButtons)
    {
        if (buttonObject)
        {
            auto audioSource = buttonObject->getComponent<AudioSource>();
            if (audioSource)
            {
                audioSource->stop();
                audioSource->cleanup();
            }
            buttonObject->destroy();
            delete buttonObject;
        }
    }

    musicPadButtons.clear();
    buttonAudioMap.clear();
    audioSets.clear();

    TextureAllocator::returnTexture(tutorialtexture);
    TextureAllocator::releaseUnusedTextures();
}

void GameScene::initializeAudioSets()
{
    audioSets.clear();

    // Define multiple audio sets
    audioSets = {
        { "../assets/Assets/Game/SFX/MusicPad0_Set1.mp3", "../assets/Assets/Game/SFX/MusicPad1_Set1.mp3",
          "../assets/Assets/Game/SFX/MusicPad2_Set1.mp3", "../assets/Assets/Game/SFX/MusicPad3_Set1.mp3",
          "../assets/Assets/Game/SFX/MusicPad4_Set1.mp3", "../assets/Assets/Game/SFX/MusicPad5_Set1.mp3" },

        { "../assets/Assets/Game/SFX/MusicPad0_Set2.mp3", "../assets/Assets/Game/SFX/MusicPad1_Set2.mp3",
          "../assets/Assets/Game/SFX/MusicPad2_Set2.mp3", "../assets/Assets/Game/SFX/MusicPad3_Set2.mp3",
          "../assets/Assets/Game/SFX/MusicPad4_Set2.mp3", "../assets/Assets/Game/SFX/MusicPad5_Set2.mp3" },
    };
}

void GameScene::applyAudioSet(int setIndex)
{
    // Clamp the index within valid range
    setIndex = std::max(0, std::min(setIndex, (int)audioSets.size() - 1));
    currentAudioSetIndex = setIndex;

    // Update audio sources for each button
    const auto& audioSet = audioSets[currentAudioSetIndex];
    for (size_t i = 0; i < musicPadButtons.size(); ++i)
    {
        int index = static_cast<int>(i); // Explicit conversion to int
        if (index < audioSet.size())
        {
            auto* audioSource = buttonAudioMap[index];
            if (audioSource)
            {
                audioSource->load(audioSet[index]); // Load the new audio file
                audioSource->setLooping(true); // Set looping to true
            }
        }
        else
        {
            std::cerr << "Index out of bounds for audioSet: " << index << std::endl;
        }
    }
}
