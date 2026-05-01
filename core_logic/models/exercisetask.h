// File: core_logic/models/exercisetask.h
#ifndef EXERCISETASK_H
#define EXERCISETASK_H

#include "healthtask.h" // Include the base class header

class ExerciseTask : public HealthTask { // Inherit publicly from HealthTask
private:
    std::string exerciseType; // e.g., "Running", "Weightlifting", "Yoga"
    int durationMinutes;

public:
    ExerciseTask(int id, const std::string& title, const std::string& description,
                 const std::string& exerciseType, int durationMinutes);

    // Override the pure virtual functions from HealthTask
    std::string getTaskType() const override;
    std::string displayTaskDetails() const override;
    std::string getCheerMessage() const override;

    // Specific getters for ExerciseTask
    std::string getExerciseType() const;
    int getDurationMinutes() const;

    // Specific setters for ExerciseTask (optional)
    void setExerciseType(const std::string& type);
    void setDurationMinutes(int minutes);
};

#endif // EXERCISETASK_H
