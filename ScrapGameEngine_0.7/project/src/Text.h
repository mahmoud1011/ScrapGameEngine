#pragma once

#include "BaseComponent.h"
#include <unordered_map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <ft2build.h>

#include FT_FREETYPE_H

namespace ScrapGameEngine {

    /**
     * @struct Glyph
     * @brief Represents a character glyph in the texture atlas.
     *
     * This structure holds information about a character glyph, including its
     * size, texture coordinates, advance, and offset.
     */
    struct Glyph
    {
        glm::vec2 textureCoords; ///< Texture coordinates for the glyph in the atlas
        glm::vec2 size;          ///< Size of the glyph (width, height)
        glm::vec2 advance;       ///< The distance to move to the next glyph
        glm::vec2 offset;        ///< Offset from the baseline to render the glyph
    };

    /**
     * @class Text
     * @brief Manages and renders text using a TrueType font.
     *
     * This component handles loading a TrueType font, creating a texture atlas for
     * glyphs, and rendering text in the game. It supports setting text content,
     * position, scale, and color.
     */
    class Text : public BaseComponent
    {
    public:
        /**
         * @brief Constructs a Text component.
         * @param owner Pointer to the associated GameObject.
         */
        explicit Text(GameObject* owner);

        /**
         * @brief Destructor.
         */
        ~Text();

        /**
         * @brief Renders the text.
         */
        void render() override;

        /**
         * @brief Sets the font to be used for rendering the text.
         * @param filePath File path to the TrueType font.
         */
        void SetFont(const std::string& filePath);

        /**
         * @brief Sets the text to be displayed.
         * @param newText The string to be displayed.
         */
        void setText(const std::string& newText);

        /**
         * @brief Sets the position of the text in the game world.
         * @param x X-coordinate.
         * @param y Y-coordinate.
         */
        void setPosition(float x, float y);

        /**
         * @brief Sets the scale of the text.
         * @param newScale Scale factor for the text size.
         */
        void setScale(float newScale);

        /**
         * @brief Sets the color of the text.
         * @param r Red component (0.0 to 1.0).
         * @param g Green component (0.0 to 1.0).
         * @param b Blue component (0.0 to 1.0).
         * @param a Alpha (transparency) component (0.0 to 1.0).
         */
        void setColor(float r, float g, float b, float a);

        /**
         * @brief Sets the color of the text.
         * @param r Red component (0.0 to 1.0).
         * @param g Green component (0.0 to 1.0).
         * @param b Blue component (0.0 to 1.0).
         * @param a Alpha (transparency) component (0.0 to 1.0).
         */
        void setColour(float r, float g, float b, float a);

    private:
        /**
         * @brief Loads the font from a TrueType font file.
         * @param filePath File path to the font file.
         * @param fontSize Size of the font.
         */
        void loadFont(const std::string& filePath, int fontSize);

        FT_Library ft;                 ///< FreeType library handle
        FT_Face face;                   ///< FreeType face (font) handle
        std::unordered_map<char, Glyph> glyphs; ///< Map of character glyphs
        unsigned int textureAtlasID;   ///< ID of the texture atlas for glyphs
        int atlasWidth;                 ///< Width of the texture atlas
        int atlasHeight;                ///< Height of the texture atlas

        std::string text;               ///< The text string to display
        glm::vec2 position;              ///< Position of the text
        float scale;                     ///< Scale factor for text size
        glm::vec4 color;                  ///< Color of the text (RGBA)
    };
}
