// File: core_logic/models/healthscore.cpp
#include "healthscore.h"
#include "userprofile.h"
#include "../managers/todomanager.h"
#include "healthtask.h"

#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <cmath>

HealthScore::HealthScore()
    : overallScore(0), profileCompletenessScore(0),
      taskCompletionScore(0), activityScore(0), wellnessScore(0) {
}

void HealthScore::calculate(const UserProfile* profile, const ToDoManager* taskManager) {
    profileCompletenessScore = 0;
    taskCompletionScore = 0;
    activityScore = 0;
    wellnessScore = 0;

    // === 1. Profile Completeness (0-25) ===
    if (profile) {
        int filledFields = 0;
        if (!profile->getName().empty()) filledFields++;
        if (profile->getAge() > 0) filledFields++;
        if (!profile->getGender().empty()) filledFields++;
        if (profile->getWeightLb() > 0) filledFields++;
        if (!profile->getHeightFtIn().empty()) filledFields++;
        if (!profile->getHealthConditions().empty()) filledFields++;
        if (!profile->getHealthGoal().empty()) filledFields++;
        if (profile->getPreferredExerciseDurationMins() > 0) filledFields++;
        // 8 fields total → scale to 25
        profileCompletenessScore = std::min(25, (filledFields * 25) / 8);
    }

    // === 2. Task Completion Rate (0-25) ===
    if (taskManager) {
        std::vector<HealthTask*> allTasks = taskManager->getAllTasks();
        if (!allTasks.empty()) {
            int completed = 0;
            for (const HealthTask* t : allTasks) {
                if (t && t->getStatus()) completed++;
            }
            double rate = static_cast<double>(completed) / allTasks.size();
            taskCompletionScore = static_cast<int>(rate * 25.0);
        } else {
            taskCompletionScore = 5; // Baseline for having an empty list (just started)
        }
    }

    // === 3. Activity Score (0-25) ===
    // Based on exercise tasks existing and completion
    if (taskManager) {
        std::vector<HealthTask*> allTasks = taskManager->getAllTasks();
        int exerciseTotal = 0;
        int exerciseCompleted = 0;
        int totalExerciseMinutes = 0;

        for (const HealthTask* t : allTasks) {
            if (t && t->getTaskType() == "Exercise") {
                exerciseTotal++;
                if (t->getStatus()) {
                    exerciseCompleted++;
                    // Try to get duration — cast to ExerciseTask
                    // Since we can't include exercisetask.h here without circular dep,
                    // we'll use a simpler heuristic
                    totalExerciseMinutes += 30; // Estimate 30 min per exercise
                }
            }
        }

        if (exerciseTotal > 0) {
            double exerciseRate = static_cast<double>(exerciseCompleted) / exerciseTotal;
            activityScore = static_cast<int>(exerciseRate * 20.0);
            // Bonus for having exercise tasks at all
            activityScore += 5;
            activityScore = std::min(25, activityScore);
        } else {
            activityScore = 3; // Low score if no exercise tasks
        }
    }

    // === 4. Wellness Score (0-25) ===
    // Based on diet tasks + overall health engagement
    if (taskManager) {
        std::vector<HealthTask*> allTasks = taskManager->getAllTasks();
        int dietTotal = 0;
        int dietCompleted = 0;
        int genericCompleted = 0;

        for (const HealthTask* t : allTasks) {
            if (t && t->getTaskType() == "Diet") {
                dietTotal++;
                if (t->getStatus()) dietCompleted++;
            }
            if (t && t->getTaskType() == "Generic" && t->getStatus()) {
                genericCompleted++;
            }
        }

        if (dietTotal > 0) {
            double dietRate = static_cast<double>(dietCompleted) / dietTotal;
            wellnessScore = static_cast<int>(dietRate * 15.0);
        }
        // Bonus for generic task completion (general health engagement)
        wellnessScore += std::min(10, genericCompleted * 3);
        wellnessScore = std::min(25, wellnessScore);
    }

    // If profile has a health goal set, small bonus to wellness
    if (profile && !profile->getHealthGoal().empty()) {
        wellnessScore = std::min(25, wellnessScore + 3);
    }

    // === Calculate Overall ===
    overallScore = profileCompletenessScore + taskCompletionScore + activityScore + wellnessScore;
    overallScore = std::max(0, std::min(100, overallScore));

    // Timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &time_t_now);
#else
    localtime_r(&time_t_now, &local_tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    lastCalculated = oss.str();

    determineCategory();
}

void HealthScore::determineCategory() {
    if (overallScore >= 80)      scoreCategory = "Excellent";
    else if (overallScore >= 60) scoreCategory = "Good";
    else if (overallScore >= 40) scoreCategory = "Fair";
    else if (overallScore >= 20) scoreCategory = "Needs Attention";
    else                         scoreCategory = "Getting Started";
}

// Getters
int HealthScore::getOverallScore() const { return overallScore; }
int HealthScore::getProfileCompletenessScore() const { return profileCompletenessScore; }
int HealthScore::getTaskCompletionScore() const { return taskCompletionScore; }
int HealthScore::getActivityScore() const { return activityScore; }
int HealthScore::getWellnessScore() const { return wellnessScore; }
std::string HealthScore::getLastCalculated() const { return lastCalculated; }
std::string HealthScore::getScoreCategory() const { return scoreCategory; }

std::string HealthScore::getScoreColor() const {
    if (overallScore >= 80)      return "#06D6A0"; // Bright green
    else if (overallScore >= 60) return "#14B8A6"; // Teal
    else if (overallScore >= 40) return "#FBBF24"; // Amber
    else if (overallScore >= 20) return "#F97316"; // Orange
    else                         return "#FF6B6B"; // Red
}

std::string HealthScore::getScoreSummary() const {
    std::ostringstream oss;
    oss << "Your Health Score: " << overallScore << "/100 — " << scoreCategory << "\n\n";
    oss << "📋 Profile Completeness: " << profileCompletenessScore << "/25\n";
    oss << "✅ Task Completion:      " << taskCompletionScore << "/25\n";
    oss << "🏃 Activity Level:       " << activityScore << "/25\n";
    oss << "🥗 Wellness & Diet:      " << wellnessScore << "/25\n";

    if (overallScore >= 80)
        oss << "\nOutstanding! You're taking excellent care of your health.";
    else if (overallScore >= 60)
        oss << "\nGreat progress! Keep up the healthy habits.";
    else if (overallScore >= 40)
        oss << "\nYou're on the right track. Try completing more tasks to boost your score.";
    else
        oss << "\nLet's get started! Try adding some health tasks and completing them.";

    return oss.str();
}

std::string HealthScore::serialize() const {
    std::ostringstream oss;
    oss << "overall:" << overallScore << "\n";
    oss << "profile:" << profileCompletenessScore << "\n";
    oss << "tasks:" << taskCompletionScore << "\n";
    oss << "activity:" << activityScore << "\n";
    oss << "wellness:" << wellnessScore << "\n";
    oss << "calculated:" << lastCalculated << "\n";
    oss << "category:" << scoreCategory << "\n";
    return oss.str();
}

void HealthScore::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string line;
    while (std::getline(iss, line)) {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        try {
            if (key == "overall")       overallScore = std::stoi(value);
            else if (key == "profile")  profileCompletenessScore = std::stoi(value);
            else if (key == "tasks")    taskCompletionScore = std::stoi(value);
            else if (key == "activity") activityScore = std::stoi(value);
            else if (key == "wellness") wellnessScore = std::stoi(value);
            else if (key == "calculated") lastCalculated = value;
            else if (key == "category")   scoreCategory = value;
        } catch (...) {
            // Silently skip malformed entries
        }
    }
}
