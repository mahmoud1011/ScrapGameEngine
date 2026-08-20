#pragma once

#include <memory>

namespace Scrap { class Project; }

namespace Scrap::Editor
{
    /**
     * @class ProjectHub
     * @brief The launcher shown before the editor opens.
     *
     * Runs its own small window and message loop, returning the chosen project. The
     * editor proper is only constructed afterwards, so it never has to cope with
     * "no project open" as a state - which is the arrangement Unity Hub and the
     * Unreal project browser both use, and for the same reason.
     */
    class ProjectHub
    {
    public:
        ProjectHub() = delete;

        /**
         * @brief Shows the hub and blocks until a project is chosen or the window closes.
         * @return The chosen project, or null when the user closed the hub.
         */
        static std::shared_ptr<Project> run();
    };
}
