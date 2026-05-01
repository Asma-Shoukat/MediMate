// File: core_logic/models/diettask.h
#ifndef DIETTASK_H
#define DIETTASK_H

#include "healthtask.h" // Include the base class header
#include <vector>       // To store a list of food items

class DietTask : public HealthTask { // Inherit publicly from HealthTask
private:
    std::string mealType;      // e.g., "Breakfast", "Lunch", "Dinner", "Snack"
    std::vector<std::string> foodItems; // List of foods for the meal
    int calorieGoal;           // Optional: calorie target for this meal/task

public:
    DietTask(int id, const std::string& title, const std::string& description,
             const std::string& mealType, int calorieGoal = 0); // calorieGoal is optional

    // Override the pure virtual functions from HealthTask
    std::string getTaskType() const override;
    std::string displayTaskDetails() const override;
    std::string getCheerMessage() const override;

    // Specific methods for DietTask
    void addFoodItem(const std::string& item);
    std::vector<std::string> getFoodItems() const;
    std::string getMealType() const;
    int getCalorieGoal() const;

    void setMealType(const std::string& type);
    void setCalorieGoal(int calories);
};

#endif // DIETTASK_H
