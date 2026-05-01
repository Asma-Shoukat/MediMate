// File: core_logic/models/healthtask.h
#ifndef HEALTHTASK_H
#define HEALTHTASK_H

#include <string>
#include <vector> // For potential future use, or if cheerUser returns multiple lines

class HealthTask {
protected: // Protected so derived classes can access them if necessary
    int taskID;
    std::string title;
    std::string description;
    bool status; // true if completed, false otherwise

public:
    // Constructor
    HealthTask(int id, const std::string& title, const std::string& description);

    // Virtual destructor (important for base classes with virtual functions)
    virtual ~HealthTask() = default; // Use default if no special cleanup needed

    // Getters
    int getTaskID() const;
    std::string getTitle() const;
    std::string getDescription() const;
    bool getStatus() const;

    // Setters / Modifiers
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void completeTask(); // Sets status to true
    void resetTask();    // Sets status to false (optional, but can be useful)

    // Pure virtual functions - these MUST be implemented by derived classes
    // This makes HealthTask an "abstract class"
    virtual std::string getTaskType() const = 0;    // e.g., "Exercise", "Diet"
    virtual std::string displayTaskDetails() const = 0; // Displays specific details of the task
    virtual std::string getCheerMessage() const = 0;    // Returns a cheer message specific to the task type

    // A common display function that can use the virtual functions
    std::string getFullTaskDisplay() const;
};

#endif // HEALTHTASK_H
