#include "renderer/SpriteRenderer.h"
#include "renderer/Graphics.h"


ScrapGameEngine::SpriteRenderer::SpriteRenderer(GameObject* owner) 
    : BaseComponent(owner), _color(glm::vec3(1.0f, 1.0f, 1.0f)), _opacity(1.0f), _size(1.0f, 1.0f), _pivot(0.5f, 0.5f)
{   }

void ScrapGameEngine::SpriteRenderer::awake()
{
    // Nothing to allocate: the batcher builds quad geometry, so a sprite no longer
    // owns a Mesh of its own. This used to create a six-vertex VBO per sprite.
}


void ScrapGameEngine::SpriteRenderer::render()
{
    const glm::vec2 position = gameObject->transform->getPosition();
    const float rotation = gameObject->transform->getLocalRotation();
    const glm::vec2 scale = gameObject->transform->getLocalScale();

    RenderParams params{};
    params.tint = { _color, _opacity };
    params.translation = { position.x * _pivot.x, position.y * _pivot.y, 0.0f };
    params.rotationZ = rotation;
    params.scale = { scale.x * _size.x, scale.y * _size.y, 1.0f };
    params.texture = _texture;

    Graphics::drawQuad(params);
}


void ScrapGameEngine::SpriteRenderer::setColour(float r, float g, float b)
{
    _color = glm::vec3(r, g, b);
}

void ScrapGameEngine::SpriteRenderer::setColour(const glm::vec3& color)
{
    _color = color;
}

void ScrapGameEngine::SpriteRenderer::setOpacity(float opacity)
{
    _opacity = glm::clamp(opacity, 0.0f, 1.0f);
}

void ScrapGameEngine::SpriteRenderer::setSize(float w, float h)
{
    _size = glm::vec2(w, h);
}

void ScrapGameEngine::SpriteRenderer::setSize(const glm::vec2& size)
{
    _size = size;
}

void ScrapGameEngine::SpriteRenderer::setPivot(float x, float y)
{
    _pivot = glm::vec2(x, y);
}

void ScrapGameEngine::SpriteRenderer::setPivot(const glm::vec2& pivot)
{
    _pivot = pivot;
}

void ScrapGameEngine::SpriteRenderer::setTexture(Texture2D* texture)
{
    _texture = texture;
}

ScrapGameEngine::Texture2D* ScrapGameEngine::SpriteRenderer::getTexture()
{
    return _texture;
}

