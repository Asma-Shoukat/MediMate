// File: core_logic/managers/todomanager.cpp
#include "todomanager.h"
#include "../models/exercisetask.h" // Needed for downcasting in save/load
#include "../models/diettask.h"     // Needed for downcasting in save/load
#include "../models/generictask.h"  // <-- ADDED THIS INCLUDE
#include <fstream>                  // For std::ofstream and std::ifstream
#include <sstream>                  // For std::stringstream
#include <iostream>                 // For std::cerr and std::cout (debug)
#include <algorithm>                // For std::find_if (already used but good to note)
#include <stdexcept>                // For std::stoi, etc., for robust error handling

// Constructor
ToDoManager::ToDoManager() : nextTaskID(1) {
    // Initialize nextTaskID to 1 (or load from a settings file in a real app)
}

// Destructor
ToDoManager::~ToDoManager() {
    for (HealthTask* task : tasks) {
        delete task; // Delete each dynamically allocated task
    }
    tasks.clear(); // Clear the vector of pointers
}

// Private helper to find task iterator
std::vector<HealthTask*>::iterator ToDoManager::findTaskIterator(int taskID) {
    return std::find_if(tasks.begin(), tasks.end(),
                        [taskID](const HealthTask* task) {
                            // Ensure task is not null before accessing its ID
                            return task && task->getTaskID() == taskID;
                        });
}

// Add a task (takes ownership)
void ToDoManager::addTask(HealthTask* task) {
    if (task) {
        tasks.push_back(task);
    }
}

// Remove a task by ID
bool ToDoManager::removeTask(int taskID) {
    auto it = findTaskIterator(taskID);
    if (it != tasks.end()) {
        delete *it;
        tasks.erase(it);
        return true;
    }
    return false;
}

// Get a task by ID (returns non-owning pointer)
HealthTask* ToDoManager::getTask(int taskID) const {
    auto it = std::find_if(tasks.begin(), tasks.end(),
                           [taskID](const HealthTask* task) {
                               return task && task->getTaskID() == taskID;
                           });
    if (it != tasks.end()) {
        return *it;
    }
    return nullptr;
}

// Mark a task as complete
void ToDoManager::completeTask(int taskID) {
    HealthTask* task = getTask(taskID);
    if (task) {
        task->completeTask(); // Assumes HealthTask has this method
    }
}

// Get all tasks (returns a vector of non-owning pointers)
std::vector<HealthTask*> ToDoManager::getAllTasks() const {
    return tasks;
}

// Generate a new unique task ID
int ToDoManager::generateNewTaskID() {
    return nextTaskID++;
}

// --- Persistence Methods ---

bool ToDoManager::saveTasksToFile(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open tasks file for writing: " << filename << std::endl;
        return false;
    }

    for (const HealthTask* task : tasks) {
        if (!task) continue;

        outFile << "TASK_TYPE:" << task->getTaskType() << "\n"; // Relies on getTaskType() returning "Exercise", "Diet", "Generic"
        outFile << "id:" << task->getTaskID() << "\n";
        outFile << "title:" << task->getTitle() << "\n";
        outFile << "description:" << task->getDescription() << "\n";
        outFile << "status:" << (task->getStatus() ? 1 : 0) << "\n"; // Assumes HealthTask has getStatus()

        // No need for dynamic_cast if getTaskType() is reliable
        if (task->getTaskType() == "Exercise") {
            const ExerciseTask* exerciseTask = static_cast<const ExerciseTask*>(task);
            outFile << "exercise_type:" << exerciseTask->getExerciseType() << "\n";
            outFile << "duration_minutes:" << exerciseTask->getDurationMinutes() << "\n";
        } else if (task->getTaskType() == "Diet") {
            const DietTask* dietTask = static_cast<const DietTask*>(task);
            outFile << "meal_type:" << dietTask->getMealType() << "\n";
            outFile << "calorie_goal:" << dietTask->getCalorieGoal() << "\n";
            for (const std::string& item : dietTask->getFoodItems()) {
                outFile << "food_item:" << item << "\n";
            }
        } else if (task->getTaskType() == "Generic") {
            // GenericTask has no additional specific fields to save beyond common ones
        }
        // Add more else if blocks for other task types here (e.g., MedicationTask)

        outFile << "\n"; // Blank line to separate tasks
    }

    if (outFile.fail()) {
        std::cerr << "Error: Failed to write all task data to file: " << filename << std::endl;
        outFile.close();
        return false;
    }

    outFile.close();
    return true;
}


// Local helper trim function
namespace {
std::string trim_local_tdm(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (std::string::npos == start) return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}
}

bool ToDoManager::loadTasksFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    for (HealthTask* task : tasks) {
        delete task;
    }
    tasks.clear();
    nextTaskID = 1;

    std::string line;
    std::string currentTaskType;
    int id = 0;
    std::string title, description;
    bool status = false;
    // Exercise specific
    std::string exerciseType;
    int durationMinutes = 0;
    // Diet specific
    std::string mealType;
    int calorieGoal = 0;
    std::vector<std::string> foodItems;

    int maxIdFound = 0;

    auto resetTempVariables = [&]() { // Lambda to reset variables for each new task
        currentTaskType.clear();
        id = 0;
        title.clear();
        description.clear();
        status = false;
        exerciseType.clear();
        durationMinutes = 0;
        mealType.clear();
        calorieGoal = 0;
        foodItems.clear();
    };

    resetTempVariables(); // Initial reset

    while (std::getline(inFile, line)) {
        line = trim_local_tdm(line);
        if (line.empty()) { // Task delimiter or end of task block
            if (!currentTaskType.empty()) { // Only process if we have a task type
                HealthTask* newTask = nullptr;
                try { // Add try-catch around task creation for safety
                    if (currentTaskType == "Exercise") {
                        newTask = new ExerciseTask(id, title, description, exerciseType, durationMinutes);
                    } else if (currentTaskType == "Diet") {
                        DietTask* dietTask = new DietTask(id, title, description, mealType, calorieGoal);
                        for (const std::string& item : foodItems) {
                            dietTask->addFoodItem(item);
                        }
                        newTask = dietTask;
                    } else if (currentTaskType == "Generic") { // <-- MODIFICATION
                        newTask = new GenericTask(id, title, description);
                    }
                    // Add MedicationTask here too if you implement it later

                    if (newTask) {
                        if (status) newTask->completeTask(); // Assumes HealthTask has completeTask()
                        tasks.push_back(newTask);
                        if (id > maxIdFound) maxIdFound = id;
                    } else if (!currentTaskType.empty()) { // If type was known but task not created
                        std::cerr << "Warning: Could not create task of known type: " << currentTaskType << " with ID " << id << std::endl;
                    }
                } catch (const std::bad_alloc& e) {
                    std::cerr << "Error: Memory allocation failed for task ID " << id << " - " << e.what() << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error: Exception during task creation for ID " << id << " - " << e.what() << std::endl;
                }
                resetTempVariables(); // Reset for the next task block
            }
            continue;
        }

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            std::cerr << "Warning: Malformed line (no colon): " << line << std::endl;
            continue;
        }

        std::string key = trim_local_tdm(line.substr(0, colonPos));
        std::string value = trim_local_tdm(line.substr(colonPos + 1));

        try { // Add try-catch around value parsing
            if (key == "TASK_TYPE") currentTaskType = value;
            else if (key == "id") id = std::stoi(value);
            else if (key == "title") title = value;
            else if (key == "description") description = value;
            else if (key == "status") status = (std::stoi(value) == 1);
            // ExerciseTask specific fields
            else if (key == "exercise_type") exerciseType = value;
            else if (key == "duration_minutes") durationMinutes = std::stoi(value);
            // DietTask specific fields
            else if (key == "meal_type") mealType = value;
            else if (key == "calorie_goal") calorieGoal = std::stoi(value);
            else if (key == "food_item") foodItems.push_back(value);
            // GenericTask has no specific extra fields to load
        } catch (const std::invalid_argument& ia) {
            std::cerr << "Warning: Invalid argument for key '" << key << "': " << value << " - " << ia.what() << std::endl;
        } catch (const std::out_of_range& oor) {
            std::cerr << "Warning: Value out of range for key '" << key << "': " << value << " - " << oor.what() << std::endl;
        }
    }

    // Process the very last task if the file doesn't end with a blank line
    // (This logic is now handled by the resetTempVariables and processing in the loop
    // if the last task block is followed by EOF instead of a blank line,
    // but it's safer to have an explicit check or ensure file ends with blank line)
    // The previous explicit block for the last task is fine too.
    // Let's re-add it for robustness in case the last data block isn't followed by an empty line.
    if (!currentTaskType.empty()) {
        HealthTask* newTask = nullptr;
        try {
            if (currentTaskType == "Exercise") {
                newTask = new ExerciseTask(id, title, description, exerciseType, durationMinutes);
            } else if (currentTaskType == "Diet") {
                DietTask* dietTask = new DietTask(id, title, description, mealType, calorieGoal);
                for (const std::string& item : foodItems) {
                    dietTask->addFoodItem(item);
                }
                newTask = dietTask;
            } else if (currentTaskType == "Generic") { // <-- MODIFICATION
                newTask = new GenericTask(id, title, description);
            }

            if (newTask) {
                if (status) newTask->completeTask();
                tasks.push_back(newTask);
                if (id > maxIdFound) maxIdFound = id;
            } else if (!currentTaskType.empty()){
                std::cerr << "Warning: Could not create last task of known type: " << currentTaskType << " with ID " << id << std::endl;
            }
        } catch (const std::bad_alloc& e) {
            std::cerr << "Error: Memory allocation failed for last task ID " << id << " - " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: Exception during last task creation for ID " << id << " - " << e.what() << std::endl;
        }
    }


    nextTaskID = maxIdFound + 1;

    if (inFile.bad() && !inFile.eof()) {
        std::cerr << "Error: A critical error occurred while reading tasks file: " << filename << std::endl;
        inFile.close();
        return false;
    }

    inFile.close();
    return true;
}
