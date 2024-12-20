#include "Scheduler.h"
#include <iostream> 

namespace ScrapGameEngine
{
    // Static variables
    std::vector<std::pair<std::function<void()>, float>> Scheduler::delayedTasks;
    std::vector<std::function<bool()>> Scheduler::repeatingTasks;

    void Scheduler::addDelayedTask(std::function<void()> task, float delay)
    {
        delayedTasks.emplace_back(task, delay);

        std::cout << "[SCHEDULER] Added new Delayed Task: " << delay << " seconds." << std::endl;
    }

    void Scheduler::addRepeatingTask(std::function<bool()> task)
    {
        repeatingTasks.push_back(task);

        std::cout << "[SCHEDULER] Added new Repeating Task." << std::endl;
    }

    void Scheduler::update(float deltaTime)
    {
        // Handle delayed tasks
        for (auto it = delayedTasks.begin(); it != delayedTasks.end();)
        {
            it->second -= deltaTime; // Reduce the delay time
            if (it->second <= 0.0f)
            {
                it->first(); // Execute the task
                it = delayedTasks.erase(it); // Remove the task
            }
            else
            {
                ++it;
            }
        }

        // Handle repeating tasks
        for (auto it = repeatingTasks.begin(); it != repeatingTasks.end();)
        {
            if ((*it)()) // If the task returns true, stop repeating
            {
                it = repeatingTasks.erase(it); // Remove the task
            }
            else
            {
                ++it;
            }
        }
    }

    void Scheduler::dispose()
    {  
        // Clear all tasks
        delayedTasks.clear();
        repeatingTasks.clear();
        std::cout << "[SCHEDULER] Disposed all tasks." << std::endl;
    }
}
