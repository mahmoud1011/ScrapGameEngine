#include <glad/glad.h>
#include "SceneStateMachine.h"
#include "Graphics.h"
#include "Input.h"
#include "TextureAllocator.h"
#include "MeshAllocater.h"
#include <iostream>
#include "MainMenuScene.h"
#include "TweenComponent.h"
#include "LoadScene.h"
#include "SplashScreenScene.h"

using namespace ScrapGameEngine;

std::unique_ptr<GameObject> SplashScreenScene::gameObject = nullptr;
std::unique_ptr<GameObject> SplashScreenScene::gameObject1 = nullptr;  

void SplashScreenScene::onInitialize()
{
    time = 0.0f;

    // Initialize vertices for the _mesh
    std::vector<Vertex> vertices;
    vertices.push_back(Vertex({ -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f })); // Top-left
    vertices.push_back(Vertex({ 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f }));  // Top-right
    vertices.push_back(Vertex({ 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f })); // Bottom-right

    vertices.push_back(Vertex({ 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f })); // Bottom-right
    vertices.push_back(Vertex({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f })); // Bottom-left
    vertices.push_back(Vertex({ -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f })); // Top-left

    _mesh = MeshAllocator::getMesh(vertices);
    mesh1 = MeshAllocator::getMesh(vertices);

    // Texture configuration for texture
    TextureConfig cfg{};
    cfg.wrapModeX = TextureWrapMode::CLAMP_TO_BORDER;
    cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
    cfg.filterMode = TextureFilterMode::NEAREST;
    cfg.borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };
    cfg.generateMipmaps = true;

    std::string texturePath = "../assets/icons/ScrapLogo_Transparent.png";
    texture = TextureAllocator::getTexture(texturePath, cfg);

    std::string texturePath1 = "../assets/info/MyInfo.png";
    texture1 = TextureAllocator::getTexture(texturePath1, cfg);
    std::cout << "Texture 1 ID: " << texture->getID() << std::endl;
    std::cout << "Texture 2 ID: " << texture1->getID() << std::endl;

    // Initialize the GameObject with a position and rotation
    gameObject = std::make_unique<GameObject>();
    gameObject->setName("Logo GameObject");
    gameObject->transform->setPosition({ 0.0f, 0.3f });
    gameObject->transform->setRotation(0.0f);        
    gameObject->transform->setScale({ 1.0f, 1.0f });

    // Add a TweenComponent to the GameObject
    TweenComponent* tweenComp = gameObject->addComponent<TweenComponent>();

    tweenComp->startTween(ScrapGameEngine::TweenType::SCALE, { 1.0f, 1.0f, 1.0f }, 2.0f, ScrapGameEngine::EasingType::EASE_IN_OUT);

    gameObject1 = std::make_unique<GameObject>();
    gameObject->setName("MyInfo GameObject");
    gameObject1->transform->setPosition({ 0.0f, -0.65f });
    gameObject1->transform->setRotation(0.0f);           
    gameObject1->transform->setScale({ 1.5f, 0.45f });
}

void SplashScreenScene::onUpdate(float deltaTime)
{
	time += deltaTime;

    auto tween = gameObject->getComponent<TweenComponent>();
    if (tween)
    {
        tween->update(deltaTime);
    }

    if (Input::getKey(KeyCode::SPACE))
    {
		SceneStateMachine::loadScene("LoadScene");
    }

	if (time > 3.0f)
		SceneStateMachine::loadScene("LoadScene");
}

void SplashScreenScene::onRender()
{
    glm::vec2 pos = gameObject->transform->getPosition();
    float rot = gameObject->transform->getLocalRotation();
    glm::vec2 scale = gameObject->transform->getLocalScale();

    RenderParams param{};
    param.tint = { 1.0f, 1.0f, 1.0f, 1.0f };
    param.translation = glm::vec3(pos, 0.0f);
    param.rotationZ = rot;
    param.scale = glm::vec3(scale, 0.0f);;
    param.texture = texture;

    Graphics::drawMesh(_mesh, param);

    glm::vec2 pos1 = gameObject1->transform->getPosition();
    float rot1 = gameObject1->transform->getLocalRotation();
    glm::vec2 scale1 = gameObject1->transform->getLocalScale();

    RenderParams param1{};
    param1.tint = { 1.0f, 1.0f, 1.0f, 1.0f };
    param1.translation = glm::vec3(pos1, 0.0f);
    param1.rotationZ = rot1;
    param1.scale = glm::vec3(scale1, 0.0f);;
    param1.texture = texture1;

    Graphics::drawMesh(mesh1, param1);
}

void SplashScreenScene::onDeactivate()
{
    MeshAllocator::releaseUnusedMeshes();

    TextureAllocator::returnTexture(texture);
    TextureAllocator::returnTexture(texture1);
    TextureAllocator::releaseUnusedTextures();
}