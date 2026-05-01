// File: core_logic/models/generictask.cpp
#include "generictask.h"
#include <sstream>
#include <vector>  // For storing multiple cheer messages
#include <random>  // For picking a random cheer message

// Constructor Implementation
GenericTask::GenericTask(int id, const std::string& title, const std::string& description)
    : HealthTask(id, title, description) {
}

// Override Implementations
std::string GenericTask::getTaskType() const {
    return "Generic";
}

std::string GenericTask::displayTaskDetails() const {
    return "No specific details for this general task.";
}

std::string GenericTask::getCheerMessage() const {
    if (getStatus()) { // Assuming getStatus() is available from HealthTask
        std::vector<std::string> cheers = {
            "Great job completing your task: '" + getTitle() + "'!",
            "Excellent work on finishing '" + getTitle() + "'!",
            "'" + getTitle() + "' is done! That's the way to do it!"
        };
        if (cheers.empty()) { // Fallback
            return "Task completed successfully!";
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, static_cast<int>(cheers.size() - 1));
        return cheers[distrib(gen)];
    }
    return "You can do it! Keep working on your task: '" + getTitle() + "'.";
}
