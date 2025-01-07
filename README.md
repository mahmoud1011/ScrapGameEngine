<a id="readme-top"></a>

# ScrapGameEngine

## About the Project
ScrapGameEngine is a versatile 2D game engine written in C++. It provides a robust and efficient framework to support a wide range of game development tasks, from basic sprite rendering to complex game mechanics. It is designed to be highly modular, allowing developers to integrate and extend its functionality easily. ScrapGameEngine supports efficient asset management, including graphics, sounds, and fonts, making it an ideal choice for both small indie projects and large-scale games. It leverages external libraries such as `freetype` for font rendering and `irrKlang` for sound and music playback to enhance its capabilities. The engine offers comprehensive support for scripting with its C++ API, enabling developers to write custom game logic using familiar programming patterns. ScrapGameEngine also includes an intuitive scene graph system and scripting support, making it easy to create interactive and responsive games.

## Key Features
- **Modular Architecture**: ScrapGameEngine is designed to be modular, allowing developers to add or replace components like graphics, input handling, and audio without overhauling the entire system.
- **2D Rendering**: Supports efficient 2D sprite rendering, including animations, transformations, and layering.
- **Audio**: Utilizes `irrKlang` for high-quality sound and music playback, providing a rich auditory experience.
- **Font Rendering**: Integrates `freetype` for rendering scalable fonts, ensuring high-quality text display.
- **Scripting**: Provides an easy-to-use C++ API for implementing game logic, along with support for advanced scripting through integration with third-party libraries.
- **Asset Management**: Efficiently manages game assets, including textures, sounds, and scripts, to support rapid development cycles.

## Libraries Used
- **freetype**: This library is used for font rendering, allowing the engine to render high-quality text on the screen. It supports various font formats and provides advanced text shaping features, making it suitable for displaying text in different languages and scripts.
- **irrKlang**: This library is used for sound and music playback, providing the engine with the ability to play background music, sound effects, and manage audio assets efficiently. `irrKlang` integrates well with ScrapGameEngine, allowing developers to add immersive audio experiences to their games with minimal setup.

## Example Usage
### Create a new game object
```cpp
auto* gameObject = GameObject::Create("Player");

gameObject->runRenderComponent;
gameObject->runUpdateComponent;

```
### Creating a Mesh
```cpp
// Define vertices for the mesh
std::vector<Vertex> vertices = {
    Vertex({ -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f }),
    Vertex({ 0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f }),
    Vertex({ 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }),
    Vertex({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f })
};
auto mesh = MeshAllocater->getMesh(vertices);
```

### Creating a Texture2D
```cpp
// Configure texture settings
TextureConfig cfg{};
cfg.wrapModeX = TextureWrapMode::CLAMP_TO_BORDER;
cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
cfg.filterMode = TextureFilterMode::NEAREST;
cfg.borderColor = { 0.0f, 0.0f, 0.0f, 1.0f };

// Load a texture with path
auto* texture = TextureAllocater->getTexture("../assets/Assets/Test.png", cfg);
```

### Using SpriteRenderer
```cpp
// Add a SpriteRenderer to a game object
auto* gameObject = GameObject::Create("Player");
auto* spriteRenderer = gameObject.addComponent<SpriteRenderer>();
sptiteRenderer.Init()
spriteRenderer.setTexture(texture2D);
spriteRenderer.setColor({1.0f, 1.0f, 1.0f, 1.0f});

```

### Using Scheduler Task
```cpp
// Schedule an action to move the game object after 2 seconds
scheduler.delayedtask([&]() {
    gameObject.transform->setPosition({ 1.0f, 0.0f });
}, 2.0f);
```

### Summary
This README provides a comprehensive overview of ScrapGameEngine and demonstrates how to create scenes, game objects, meshes, textures, and utilize the scheduler effectively. 

More documents to come soon!
