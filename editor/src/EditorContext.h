#pragma once

#include "EditorCamera.h"

#include <glm/glm.hpp>
#include <deque>
#include <string>

namespace ScrapGameEngine { class GameObject; }

namespace Scrap::Editor
{
    /**
     * @enum PlayState
     * @brief Whether the editor is authoring or running the scene.
     */
    enum class PlayState { Edit, Playing, Paused };

    /**
     * @enum GizmoOp
     * @brief Which transform handle the viewport gizmo shows.
     */
    enum class GizmoOp { None, Translate, Rotate, Scale };

    /**
     * @struct LogEntry
     * @brief One line in the console.
     */
    struct LogEntry
    {
        enum class Level { Info, Warning, Error };
        Level level = Level::Info;
        std::string message;
    };

    /**
     * @class EditorContext
     * @brief Editor-only state, deliberately kept out of the engine.
     *
     * Selection, gizmo mode, play state and the console buffer are things the editor
     * needs and a shipped game never does. Keeping them here rather than on the scene
     * is what stops editor concerns leaking into the runtime - the split the plan
     * flags as worth protecting early.
     */
    class EditorContext
    {
    public:
        static EditorContext& get();

        // --- selection -------------------------------------------------------
        ScrapGameEngine::GameObject* selection = nullptr;
        bool hasSelection() const { return selection != nullptr; }
        void select(ScrapGameEngine::GameObject* go) { selection = go; }
        void clearSelection() { selection = nullptr; }

        // --- viewport --------------------------------------------------------
        EditorCamera camera;
        glm::vec2 viewportSize{1280.0f, 720.0f};
        glm::vec2 viewportBoundsMin{0.0f};
        bool viewportHovered = false;
        bool viewportFocused = false;

        // --- modes -----------------------------------------------------------
        PlayState playState = PlayState::Edit;
        GizmoOp gizmoOp = GizmoOp::Translate;
        bool gizmoLocalSpace = false;
        bool showGrid = true;
        bool snapEnabled = false;
        float translateSnap = 0.25f;
        float rotateSnap = 15.0f;
        float scaleSnap = 0.1f;

        bool isPlaying() const { return playState == PlayState::Playing; }

        // --- console ---------------------------------------------------------
        void log(LogEntry::Level level, std::string message);
        void logInfo(std::string m)    { log(LogEntry::Level::Info, std::move(m)); }
        void logWarning(std::string m) { log(LogEntry::Level::Warning, std::move(m)); }
        void logError(std::string m)   { log(LogEntry::Level::Error, std::move(m)); }
        void clearLog() { entries.clear(); }
        const std::deque<LogEntry>& logEntries() const { return entries; }

        // --- frame stats -----------------------------------------------------
        float frameMs = 0.0f;
        float fps = 0.0f;

    private:
        EditorContext() = default;

        // Bounded so a script logging every frame cannot grow without limit.
        static constexpr size_t kMaxLogEntries = 500;
        std::deque<LogEntry> entries;
    };
}
