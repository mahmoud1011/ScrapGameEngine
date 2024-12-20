#pragma once
#include "BaseScene.h"
#include <glm/vec2.hpp>
#include "GameObject.h"
#include "Texture2D.h"
#include <memory>

using namespace ScrapGameEngine;

class MainMenuScene : public BaseScene
{
public:
    std::string getName() const override { return "MainMenuScene"; }
protected:

    void onInitialize() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onDeactivate() override;

    float time;

private:
    static std::unique_ptr<GameObject> gameObject;
    Texture2D* texture;
};
