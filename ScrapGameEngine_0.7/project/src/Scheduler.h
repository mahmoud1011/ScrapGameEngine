#pragma once
#include <functional>
#include <vector>
#include <utility> 

namespace ScrapGameEngine
{
    /**
     * @class Scheduler
     * @brief A task scheduler that manages delayed and repeating tasks.
     *
     * The Scheduler allows you to register tasks to run after a specified delay or repeatedly.
     * It provides methods to update the state of the scheduler by advancing the time and executing due tasks.
     */
    class Scheduler
    {
    public:
        /**
         * @brief Register a task to run after a specified delay.
         *
         * @param task The task function to be executed.
         * @param delay The time (in seconds) after which the task should run.
         */
        static void addDelayedTask(std::function<void()> task, float delay);

        /**
         * @brief Register a repeating task.
         *
         * The task function should return `true` to stop the task or `false` to continue repeating.
         *
         * @param task The task function to be executed.
         */
        static void addRepeatingTask(std::function<bool()> task);

        /**
         * @brief Update the scheduler (call this every frame) to check for due tasks.
         *
         * @param deltaTime The time (in seconds) since the last frame.
         */
        static void update(float deltaTime);

        /**
         * @brief Clean up resources (clear all tasks).
         */
        static void dispose();

    private:
        static std::vector<std::pair<std::function<void()>, float>> delayedTasks; ///< List of delayed tasks with their execution time
        static std::vector<std::function<bool()>> repeatingTasks; ///< List of repeating tasks with their stop condition
    };
}
