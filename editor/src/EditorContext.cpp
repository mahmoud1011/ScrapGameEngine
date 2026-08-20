#include "EditorContext.h"

#ifdef SCRAP_HAS_DOTNET
#include "scripting/ScriptEngine.h"
#endif

namespace Scrap::Editor
{
    EditorContext& EditorContext::get()
    {
        static EditorContext instance;
        return instance;
    }

    void EditorContext::onPlay()
    {
        // Copy-on-play: the runtime mutates a snapshot, so Stop is a pointer swap
        // rather than a reload, and edits made before Play survive.
        activeScene = Scrap::Scene::copy(editorScene);
        playState = PlayState::Playing;
        clearSelection();
#ifdef SCRAP_HAS_DOTNET
        Scrap::ScriptEngine::onRuntimeStart(activeScene.get());
#endif
    }

    void EditorContext::onStop()
    {
#ifdef SCRAP_HAS_DOTNET
        Scrap::ScriptEngine::onRuntimeStop();
#endif
        activeScene = editorScene;
        playState = PlayState::Edit;
        clearSelection();
    }

    void EditorContext::log(LogEntry::Level level, std::string message)
    {
        entries.push_back({level, std::move(message)});
        while (entries.size() > kMaxLogEntries) entries.pop_front();
    }
}
