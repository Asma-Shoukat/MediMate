// File: core_logic/models/exercisetask.cpp
#include "exercisetask.h"
#include <sstream> // For string stream formatting
#include <vector>  // For storing multiple cheer messages
#include <random>  // For picking a random cheer message

// Constructor Implementation
ExerciseTask::ExerciseTask(int id, const std::string& title, const std::string& description,
                           const std::string& exerciseType, int durationMinutes)
    : HealthTask(id, title, description),
    exerciseType(exerciseType),
    durationMinutes(durationMinutes) {
}

// Override Implementations
std::string ExerciseTask::getTaskType() const {
    return "Exercise";
}

std::string ExerciseTask::displayTaskDetails() const {
    std::stringstream ss;
    ss << "Type of Exercise: " << exerciseType << "\n"
       << "Duration: " << durationMinutes << " minutes";
    return ss.str();
}

std::string ExerciseTask::getCheerMessage() const {
    if (getStatus()) { // Check if task is completed (getStatus() from HealthTask)
        std::vector<std::string> cheers;
        if (durationMinutes >= 60) {
            cheers = {
                "Incredible stamina! You absolutely crushed that " + std::to_string(durationMinutes) + "-minute workout!",
                "Wow, a " + std::to_string(durationMinutes) + "-minute session! You're a true fitness warrior!",
                "That was a marathon effort! Super proud of your dedication!"
            };
        } else if (durationMinutes >= 30) {
            cheers = {
                "Fantastic job on your " + exerciseType + " workout! Feeling energized?",
                "Great work pushing through that " + std::to_string(durationMinutes) + "-minute exercise!",
                "You did it! Every rep, every step, every minute counts!"
            };
        } else if (durationMinutes > 0) {
            cheers = {
                "Awesome for getting that " + exerciseType + " in! Consistency is queen (or king)!",
                "Nice one! That quick burst of activity makes a world of difference!",
                "Every bit of movement is a win! Pat yourself on the back!"
            };
        } else { // durationMinutes == 0 (likely a custom task added as "General" exercise, like "drink water today")
            cheers = {
                "Task '" + getTitle() + "' completed! Excellent job staying on track!",
                "Checked off '" + getTitle() + "' from the list! Way to be proactive!",
                "Another one down! '" + getTitle() + "' is officially done and dusted!"
            };
        }

        if (cheers.empty()) { // Fallback, though should not happen with current logic
            return "Task completed! Well done!";
        }

        // Pick a random cheer from the selected pool
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, static_cast<int>(cheers.size() - 1));
        return cheers[distrib(gen)];
    }
    // Message if not completed
    return "Keep pushing towards your " + exerciseType + " goal! You're capable of amazing things.";
}

// Specific Getter Implementations
std::string ExerciseTask::getExerciseType() const {
    return exerciseType;
}

int ExerciseTask::getDurationMinutes() const {
    return durationMinutes;
}

// Specific Setter Implementations
void ExerciseTask::setExerciseType(const std::string& type) {
    this->exerciseType = type;
}

void ExerciseTask::setDurationMinutes(int minutes) {
    if (minutes >= 0) {
        this->durationMinutes = minutes;
    }
}
