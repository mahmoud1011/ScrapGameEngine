#pragma once
#include "scene/BaseScene.h"
#include "scene/GameObject.h"
#include "audio/AudioSource.h"
#include "renderer/Texture2D.h"
#include <string>
#include <unordered_map>
#include <memory>

using namespace ScrapGameEngine;

class GameScene : public BaseScene
{
public:
    std::string getName() const override { return "GameScene"; }

protected:
    void onInitialize() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onDeactivate() override;

    float time;

private:
    static std::unique_ptr<GameObject> tutorialObject;

    void initializeAudioSets();
    void applyAudioSet(int setIndex);

    std::vector<GameObject*> musicPadButtons;
    std::unordered_map<int, AudioSource*> buttonAudioMap; // Map button index to AudioSource pointers

    GameObject* clickAudioSource;
    Texture2D* tutorialtexture;
};
