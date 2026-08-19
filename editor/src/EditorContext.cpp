#include "EditorContext.h"

namespace Scrap::Editor
{
    EditorContext& EditorContext::get()
    {
        static EditorContext instance;
        return instance;
    }

    void EditorContext::log(LogEntry::Level level, std::string message)
    {
        entries.push_back({level, std::move(message)});
        while (entries.size() > kMaxLogEntries) entries.pop_front();
    }
}
