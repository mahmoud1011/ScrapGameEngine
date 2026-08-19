#pragma once

#include <filesystem>

namespace Scrap::Editor
{
    /**
     * @brief The editor's docked panels.
     *
     * Layout follows Unity's conventions - hierarchy left, inspector right, content
     * browser and console below, viewport in the centre - because that arrangement is
     * what people already know. The visual language is ScrapEngine's own.
     */
    namespace Panels
    {
        /** @brief The top strip: play controls, gizmo mode, snapping. */
        void drawToolbar();

        /** @brief The scene, drawn into the renderer's offscreen target. */
        void drawViewport();

        /** @brief The scene's object tree. */
        void drawHierarchy();

        /** @brief Components of the selected object. */
        void drawInspector();

        /** @brief Project assets on disk. */
        void drawContentBrowser();

        /** @brief Engine and script log output. */
        void drawConsole();

        /** @brief Frame timing and renderer counters. */
        void drawStats();

        /** @brief Sets the directory the content browser roots at. */
        void setContentRoot(const std::filesystem::path& root);
    }
}
