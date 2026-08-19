#pragma once

#include <imgui.h>

namespace Scrap::Editor
{
    /**
     * @brief ScrapEngine's editor palette.
     *
     * Cool slate chrome with a patina accent - oxidised copper, which is where the
     * engine's name points. The accent is used sparingly: selection, focus, the active
     * tab, and the play state. Everything else stays quiet so the viewport is the
     * brightest thing on screen.
     *
     * Semantic colours (warn/error/success) are deliberately separate from the accent
     * so a warning never reads as "selected".
     */
    struct Palette
    {
        // Chrome, darkest to lightest.
        static constexpr ImVec4 Void        {0.043f, 0.055f, 0.067f, 1.00f};
        static constexpr ImVec4 Sunken      {0.063f, 0.078f, 0.094f, 1.00f};
        static constexpr ImVec4 Ground      {0.086f, 0.106f, 0.125f, 1.00f};
        static constexpr ImVec4 Surface     {0.114f, 0.137f, 0.161f, 1.00f};
        static constexpr ImVec4 Raised      {0.149f, 0.180f, 0.208f, 1.00f};
        static constexpr ImVec4 Hover       {0.192f, 0.231f, 0.267f, 1.00f};
        static constexpr ImVec4 Line        {0.216f, 0.259f, 0.298f, 1.00f};

        // Text.
        static constexpr ImVec4 Ink         {0.878f, 0.906f, 0.933f, 1.00f};
        static constexpr ImVec4 InkMuted    {0.529f, 0.588f, 0.647f, 1.00f};
        static constexpr ImVec4 InkFaint    {0.361f, 0.408f, 0.459f, 1.00f};

        // Patina accent.
        static constexpr ImVec4 Accent      {0.180f, 0.643f, 0.588f, 1.00f};
        static constexpr ImVec4 AccentHover {0.243f, 0.741f, 0.678f, 1.00f};
        static constexpr ImVec4 AccentDim   {0.180f, 0.643f, 0.588f, 0.28f};
        static constexpr ImVec4 AccentFaint {0.180f, 0.643f, 0.588f, 0.13f};

        // Semantic - never used for selection.
        static constexpr ImVec4 Danger      {0.827f, 0.322f, 0.290f, 1.00f};
        static constexpr ImVec4 Warning     {0.847f, 0.639f, 0.235f, 1.00f};
        static constexpr ImVec4 Success     {0.353f, 0.686f, 0.435f, 1.00f};

        // Transform gizmo axes, shared by the gizmo and the inspector's XYZ fields so
        // the same axis reads the same colour everywhere.
        static constexpr ImVec4 AxisX       {0.831f, 0.353f, 0.361f, 1.00f};
        static constexpr ImVec4 AxisY       {0.478f, 0.718f, 0.353f, 1.00f};
        static constexpr ImVec4 AxisZ       {0.361f, 0.573f, 0.831f, 1.00f};
    };

    /** @brief Applies the ScrapEngine style to the current ImGui context. */
    void applyScrapTheme();
}
