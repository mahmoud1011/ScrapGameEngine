#include <iostream>
#include <glad/glad.h>
#include "GameObject.h"
#include "TextureAllocator.h"
#include "SpriteRenderer.h"
#include "Text.h"

namespace ScrapGameEngine
{
    Text::Text(GameObject* owner)
        : BaseComponent(owner), textureAtlasID(0), atlasWidth(512), atlasHeight(512), position(0.0f, 0.0f), scale(1.0f), color(1.0f, 1.0f, 1.0f, 1.0f)
    {
        if (FT_Init_FreeType(&ft))
        {
            std::cerr << "Could not initialize FreeType library" << std::endl;
        }
        else
        {
            std::cout << "[Text_Component] FreeType library initialized successfully." << std::endl;
        }
    }

    Text::~Text()
    {
        if (face)
        {
            FT_Done_Face(face);
        }

        FT_Done_FreeType(ft);
    }

    void Text::loadFont(const std::string& filePath, int fontSize)
    {
        if (FT_New_Face(ft, filePath.c_str(), 0, &face))
        {
            std::cerr << "[Text_Component] Failed to load font: " << filePath << std::endl;
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        std::vector<unsigned char> textureBuffer(atlasWidth * atlasHeight, 0);

        int padding = 2;
        int row = padding;
        int col = padding;

        for (unsigned char c = 32; c < 127; ++c)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cerr << "[Text_Component] Failed to load character: " << c << std::endl;
                continue;
            }

            if (col + face->glyph->bitmap.width + padding > atlasWidth)
            {
                col = padding;
                row += fontSize + padding;
            }

            if (row + face->glyph->bitmap.rows + padding > atlasHeight)
            {
                std::cerr << "[Text_Component] Texture atlas overflow. Increase atlas size." << std::endl;
                break;
            }

            for (unsigned int y = 0; y < face->glyph->bitmap.rows; ++y)
            {
                for (unsigned int x = 0; x < face->glyph->bitmap.width; ++x)
                {
                    textureBuffer[(row + y) * atlasWidth + (col + x)] =
                        face->glyph->bitmap.buffer[y * face->glyph->bitmap.width + x];
                }
            }

            glyphs[c] = {
                glm::vec2(col / (float)atlasWidth, row / (float)atlasHeight),
                glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::vec2((float)(face->glyph->advance.x >> 6), (float)(face->glyph->advance.y >> 6)),
                glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top)
            };

            col += face->glyph->bitmap.width + padding;
        }

        // Create and configure the texture
        TextureConfig cfg{};
        cfg.wrapModeX = TextureWrapMode::CLAMP_TO_BORDER;
        cfg.wrapModeY = TextureWrapMode::CLAMP_TO_BORDER;
        cfg.filterMode = TextureFilterMode::LINEAR;
        cfg.borderColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        cfg.generateMipmaps = false;

        std::string texturePath = "../assets/Assets/text.png";
        auto texture = TextureAllocator::getTexture(texturePath, cfg);

        if (!texture)
        {
            std::cerr << "[Text_Component] Failed to create texture atlas." << std::endl;
            return;
        }

        // Assign the texture to the SpriteRenderer
        auto spriteRenderer = gameObject->getComponent<SpriteRenderer>();
        if (!spriteRenderer)
        {
            spriteRenderer = gameObject->addComponent<SpriteRenderer>();
        }
        spriteRenderer->awake();
        spriteRenderer->setTexture(texture);
    }

    void Text::render()
    {
      
    }

    void Text::SetFont(const std::string& filePath)
    {
        loadFont(filePath, static_cast<int>(scale * 16)); // Default font size
    }

    void Text::setText(const std::string& newText)
    {
        text = newText;
    }

    void Text::setPosition(float x, float y)
    {
        position = { x, y };
    }

    void Text::setScale(float newScale)
    {
        scale = newScale;
    }

    void Text::setColor(float r, float g, float b, float a)
    {
        color = { r, g, b, a };
    }

    void Text::setColour(float r, float g, float b, float a)
    {
        color = { r, g, b, a };
    }
}
