// File: core_logic/models/healthscore.h
#ifndef HEALTHSCORE_H
#define HEALTHSCORE_H

#include <string>
#include <vector>
#include <ctime>

class UserProfile;
class ToDoManager;

class HealthScore {
private:
    int overallScore;           // 0–100 composite score
    int profileCompletenessScore;  // 0–25
    int taskCompletionScore;       // 0–25
    int activityScore;             // 0–25
    int wellnessScore;             // 0–25
    std::string lastCalculated;
    std::string scoreCategory;     // "Excellent", "Good", "Fair", "Needs Attention"

    void determineCategory();

public:
    HealthScore();

    // Calculate score from profile and task data
    void calculate(const UserProfile* profile, const ToDoManager* taskManager);

    // Getters
    int getOverallScore() const;
    int getProfileCompletenessScore() const;
    int getTaskCompletionScore() const;
    int getActivityScore() const;
    int getWellnessScore() const;
    std::string getLastCalculated() const;
    std::string getScoreCategory() const;
    std::string getScoreColor() const;     // Returns hex color for UI display
    std::string getScoreSummary() const;   // Human-readable summary

    // For persistence
    std::string serialize() const;
    void deserialize(const std::string& data);
};

#endif // HEALTHSCORE_H
