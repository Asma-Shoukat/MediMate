// File: core_logic/models/generictask.h
#ifndef GENERICTASK_H
#define GENERICTASK_H

#include "healthtask.h" // Include the base class header

class GenericTask : public HealthTask {
public:
    // Constructor
    // It will primarily use the base class constructor for its data.
    GenericTask(int id, const std::string& title, const std::string& description);

    // Override the pure virtual functions from HealthTask
    std::string getTaskType() const override;
    std::string displayTaskDetails() const override; // Provides specific details for a generic task
    std::string getCheerMessage() const override;
};

#endif // GENERICTASK_H
