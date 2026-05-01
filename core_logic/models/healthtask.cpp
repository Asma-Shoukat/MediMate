// File: core_logic/models/healthtask.cpp
#include "healthtask.h"
#include <sstream> // For string stream formatting

// Constructor Implementation
HealthTask::HealthTask(int id, const std::string& title, const std::string& description)
    : taskID(id), title(title), description(description), status(false) {
    // Initialize status to false (not completed) by default
}

// Getter Implementations
int HealthTask::getTaskID() const {
    return taskID;
}

std::string HealthTask::getTitle() const {
    return title;
}

std::string HealthTask::getDescription() const {
    return description;
}

bool HealthTask::getStatus() const {
    return status;
}

// Setter/Modifier Implementations
void HealthTask::setTitle(const std::string& newTitle) {
    this->title = newTitle;
}

void HealthTask::setDescription(const std::string& newDescription) {
    this->description = newDescription;
}

void HealthTask::completeTask() {
    status = true;
}

void HealthTask::resetTask() {
    status = false;
}

// Implementation of the common display function
std::string HealthTask::getFullTaskDisplay() const {
    std::stringstream ss;
    ss << "ID: " << taskID << "\n"
       << "Title: " << title << "\n"
       << "Type: " << getTaskType() << "\n" // Calls virtual function
       << "Description: " << description << "\n"
       << "Status: " << (status ? "Completed" : "Pending") << "\n";
    ss << "Details: " << displayTaskDetails(); // Calls virtual function
    return ss.str();
}
