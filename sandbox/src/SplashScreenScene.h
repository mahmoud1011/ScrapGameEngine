#pragma once
#include "scene/BaseScene.h"
#include <glm/vec2.hpp>
#include "renderer/Texture2D.h"
#include "scene/GameObject.h"
#include "renderer/Mesh.h"
#include <memory>

using namespace ScrapGameEngine;

class SplashScreenScene : public BaseScene
{
public:
    std::string getName() const override { return "SplashScreenScene"; }

protected:
    void onInitialize() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onDeactivate() override;

    float time;
private:
    static std::unique_ptr<GameObject> gameObject;
    static std::unique_ptr<GameObject> gameObject1;

    Mesh* _mesh;
    Mesh* mesh1;

    Texture2D* texture;
    Texture2D* texture1;
};

