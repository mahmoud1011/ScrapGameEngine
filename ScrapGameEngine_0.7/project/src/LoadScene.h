#pragma once
#include "BaseScene.h"
#include "GameObject.h"
#include <memory>

using namespace ScrapGameEngine;

class LoadScene : public BaseScene
{
public:
    std::string getName() const override { return "LoadScene"; }

    // Load a new scene by name
    void load(const std::string& sceneName);
    void setLoadTime(float time);

protected:
    void onInitialize() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onDeactivate() override;

    float time;
    void updateLoadingBar() const;
private:
    static std::unique_ptr<GameObject> gameObject;
	std::string _sceneName;

    float _loadTime; // Loading time
    float _progress; // Loading progress
};

