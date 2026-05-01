// File: core_logic/models/userprofile.h
#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <string>
#include <vector> // For potential future use with health conditions list

class UserProfile {
private:
    std::string name;
    int age;
    std::string gender;
    double weightLb; // Weight in pounds
    std::string heightFtIn; // Height as a string e.g., "5'7\""
    std::string healthConditions; // Could be a comma-separated list or simple text
    std::string healthGoal;
    int preferredExerciseDurationMins;

public:
    // Constructors
    UserProfile();
    UserProfile(const std::string& name, int age, const std::string& gender,
                double weightLb, const std::string& heightFtIn,
                const std::string& healthConditions, const std::string& healthGoal,
                int preferredExerciseDurationMins);

    // Getters (const-correct)
    std::string getName() const;
    int getAge() const;
    std::string getGender() const;
    double getWeightLb() const;
    std::string getHeightFtIn() const;
    std::string getHealthConditions() const;
    std::string getHealthGoal() const;
    int getPreferredExerciseDurationMins() const;

    // Setters
    void setName(const std::string& name);
    void setAge(int age); // Consider adding validation (e.g., age > 0)
    void setGender(const std::string& gender);
    void setWeightLb(double weightLb); // Consider validation (e.g., weight > 0)
    void setHeightFtIn(const std::string& heightFtIn);
    void setHealthConditions(const std::string& healthConditions);
    void setHealthGoal(const std::string& healthGoal);
    void setPreferredExerciseDurationMins(int duration); // Consider validation

    // Utility method to display profile information (for debugging/console use initially)
    std::string getProfileSummary() const;
};

#endif // USERPROFILE_H
