// File: core_logic/managers/todomanager.h (or core_logic/todomanager.h)
#ifndef TODOMANAGER_H
#define TODOMANAGER_H

#include "../models/healthtask.h" // Adjust path if todomanager.h is not in a 'managers' subfolder
    // If todomanager.h is in core_logic/, it would be "models/healthtask.h"
#include <vector>
#include <string>
#include <algorithm> // For std::remove_if

class ToDoManager {
private:
    std::vector<HealthTask*> tasks; // Stores pointers to allow polymorphism
    int nextTaskID;                 // To generate unique IDs for new tasks

    // Private helper to find a task by ID (returns iterator)
    std::vector<HealthTask*>::iterator findTaskIterator(int taskID);

public:
    ToDoManager();
    ~ToDoManager(); // Important: Must clean up dynamically allocated tasks

    // Core functionalities
    void addTask(HealthTask* task); // Takes ownership of the task pointer
    bool removeTask(int taskID);
    HealthTask* getTask(int taskID) const; // Returns a non-owning pointer
    void completeTask(int taskID);
    std::vector<HealthTask*> getAllTasks() const; // Returns a copy of pointers (non-owning)

    // Utility to generate a new ID
    int generateNewTaskID();

     //Persistence (declarations only for now)
     bool saveTasksToFile(const std::string& filename) const;
     bool loadTasksFromFile(const std::string& filename);
};

#endif // TODOMANAGER_H
