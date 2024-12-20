#include "Application.h"
#include "SceneStateMachine.h"
#include "SplashScreenScene.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "LoadScene.h"

int main() 
{
    Application app(600, 600, "Scrap Engine");
    int result = app.init(); // Initialize the application

    if (result == 1) 
    {
        ScrapGameEngine::SceneStateMachine::addScene<SplashScreenScene>();
        ScrapGameEngine::SceneStateMachine::addScene<MainMenuScene>();
        ScrapGameEngine::SceneStateMachine::addScene<GameScene>();
        ScrapGameEngine::SceneStateMachine::addScene<LoadScene>();

        ScrapGameEngine::SceneStateMachine::loadScene("SplashScreenScene");
        app.run(); // Run the application
    }


    return 0; 
}
