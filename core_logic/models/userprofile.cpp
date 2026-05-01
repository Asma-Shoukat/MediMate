// File: core_logic/models/userprofile.cpp
#include "userprofile.h"
#include <sstream> // For getProfileSummary
#include <stdexcept> // For potential validation errors

// Default Constructor
UserProfile::UserProfile()
    : age(0), weightLb(0.0), preferredExerciseDurationMins(0) {
    // Initialize strings to empty or default values if desired
}

// Parameterized Constructor
UserProfile::UserProfile(const std::string& name, int age, const std::string& gender,
                         double weightLb, const std::string& heightFtIn,
                         const std::string& healthConditions, const std::string& healthGoal,
                         int preferredExerciseDurationMins)
    : name(name), age(age), gender(gender), weightLb(weightLb),
    heightFtIn(heightFtIn), healthConditions(healthConditions),
    healthGoal(healthGoal), preferredExerciseDurationMins(preferredExerciseDurationMins) {
    // Basic validation can be added here or ensure setters handle it
    if (age <= 0) {
        // For simplicity, we'll just set to a default or rely on setter validation
        // In a real app, might throw or log
        this->age = 0; // Or some sensible default / throw std::invalid_argument("Age must be positive.");
    }
    if (weightLb < 0) {
        this->weightLb = 0; // Or throw
    }
    if (preferredExerciseDurationMins < 0) {
        this->preferredExerciseDurationMins = 0; // Or throw
    }
}

// Getters
std::string UserProfile::getName() const { return name; }
int UserProfile::getAge() const { return age; }
std::string UserProfile::getGender() const { return gender; }
double UserProfile::getWeightLb() const { return weightLb; }
std::string UserProfile::getHeightFtIn() const { return heightFtIn; }
std::string UserProfile::getHealthConditions() const { return healthConditions; }
std::string UserProfile::getHealthGoal() const { return healthGoal; }
int UserProfile::getPreferredExerciseDurationMins() const { return preferredExerciseDurationMins; }

// Setters
void UserProfile::setName(const std::string& name) { this->name = name; }

void UserProfile::setAge(int age) {
    if (age > 0 && age < 150) { // Basic validation
        this->age = age;
    } else {
        // Handle error: throw exception, log, or set to a default
        // For now, let's not change it if invalid to keep it simple for console
        // Or: throw std::invalid_argument("Invalid age provided.");
        // If throwing, ensure it's caught appropriately.
    }
}

void UserProfile::setGender(const std::string& gender) { this->gender = gender; }

void UserProfile::setWeightLb(double weightLb) {
    if (weightLb >= 0) { // Basic validation
        this->weightLb = weightLb;
    }
    // else handle error
}

void UserProfile::setHeightFtIn(const std::string& heightFtIn) { this->heightFtIn = heightFtIn; }

void UserProfile::setHealthConditions(const std::string& healthConditions) {
    this->healthConditions = healthConditions;
}

void UserProfile::setHealthGoal(const std::string& healthGoal) {
    this->healthGoal = healthGoal;
}

void UserProfile::setPreferredExerciseDurationMins(int duration) {
    if (duration >= 0) { // Basic validation
        this->preferredExerciseDurationMins = duration;
    }
    // else handle error
}

// Utility method
std::string UserProfile::getProfileSummary() const {
    std::stringstream ss;
    ss << "Name: " << name << "\n"
       << "Age: " << age << "\n"
       << "Gender: " << gender << "\n"
       << "Weight: " << weightLb << " lb\n"
       << "Height: " << heightFtIn << "\n"
       << "Health Conditions: " << healthConditions << "\n"
       << "Health Goal: " << healthGoal << "\n"
       << "Preferred Exercise Duration: " << preferredExerciseDurationMins << " mins\n";
    return ss.str();
}
