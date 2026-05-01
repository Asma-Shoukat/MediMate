// File: core_logic/models/diettask.cpp
#include "diettask.h"
#include <sstream> // For string stream formatting
#include <vector>  // For storing multiple cheer messages
#include <random>  // For picking a random cheer message

// Constructor Implementation
DietTask::DietTask(int id, const std::string& title, const std::string& description,
                   const std::string& mealType, int calorieGoal)
    : HealthTask(id, title, description),
    mealType(mealType),
    calorieGoal(calorieGoal) {
}

// Override Implementations
std::string DietTask::getTaskType() const {
    return "Diet";
}

std::string DietTask::displayTaskDetails() const {
    std::stringstream ss;
    ss << "Meal Type: " << mealType << "\n";
    if (calorieGoal > 0) {
        ss << "Calorie Goal: " << calorieGoal << " kcal\n";
    }
    ss << "Food Items: ";
    if (foodItems.empty()) {
        ss << "None specified.";
    } else {
        for (size_t i = 0; i < foodItems.size(); ++i) {
            ss << foodItems[i] << (i == foodItems.size() - 1 ? "" : ", ");
        }
    }
    return ss.str();
}

std::string DietTask::getCheerMessage() const {
    if (getStatus()) { // Assuming 'status' is true if task completed (getStatus() from HealthTask)
        std::vector<std::string> cheers;
        if (!foodItems.empty()) {
            cheers = {
                "Fantastic! You've nourished your body with that " + mealType + ". Healthy choices, happy you!",
                "Well done logging your " + mealType + "! Every healthy meal is a step forward.",
                "Awesome tracking for your " + mealType + "! Fueling your body right!"
            };
        } else { // No specific food items logged, but task completed
            cheers = {
                "Great, you've completed your diet task for " + mealType + "!",
                "Excellent, " + mealType + " task is checked off!",
                "Good job focusing on your " + mealType + " nutrition!"
            };
        }

        if (cheers.empty()) { // Fallback
            return "Diet task completed! You're doing great!";
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, static_cast<int>(cheers.size() - 1));
        return cheers[distrib(gen)];
    }
    // Message if not completed
    return "Remember your healthy eating goals for " + mealType + "! You've got this.";
}

// Specific Method Implementations for DietTask
void DietTask::addFoodItem(const std::string& item) {
    foodItems.push_back(item);
}

std::vector<std::string> DietTask::getFoodItems() const {
    return foodItems;
}

std::string DietTask::getMealType() const {
    return mealType;
}

int DietTask::getCalorieGoal() const {
    return calorieGoal;
}

void DietTask::setMealType(const std::string& type) {
    this->mealType = type;
}

void DietTask::setCalorieGoal(int calories) {
    if (calories >= 0) {
        this->calorieGoal = calories;
    }
}
