// File: core_logic/persistence/file_manager.cpp
#include "file_manager.h"
#include <fstream>
#include <sstream>
#include <iostream> // For error messages (can be replaced with logging)

namespace FileManager {

// Helper to trim leading/trailing whitespace (basic version)
std::string trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (std::string::npos == start) {
        return ""; // String is all whitespace
    }
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

bool saveUserProfile(const UserProfile& profile, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    outFile << "name:" << profile.getName() << "\n";
    outFile << "age:" << profile.getAge() << "\n";
    outFile << "gender:" << profile.getGender() << "\n";
    outFile << "weight_lb:" << profile.getWeightLb() << "\n";
    outFile << "height_ft_in:" << profile.getHeightFtIn() << "\n";
    outFile << "health_conditions:" << profile.getHealthConditions() << "\n";
    outFile << "health_goal:" << profile.getHealthGoal() << "\n";
    outFile << "preferred_exercise_duration_mins:" << profile.getPreferredExerciseDurationMins() << "\n";

    if (outFile.fail()) {
        std::cerr << "Error: Failed to write all data to file: " << filename << std::endl;
        outFile.close();
        return false;
    }

    outFile.close();
    return true;
}

bool loadUserProfile(UserProfile& profile, const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        // This is not necessarily an error if the file doesn't exist on first run
        // std::cerr << "Info: Could not open file for reading (may not exist yet): " << filename << std::endl;
        return false;
    }

    std::string line;
    std::string key, value;

    while (std::getline(inFile, line)) {
        std::stringstream ss(line);
        if (std::getline(ss, key, ':') && std::getline(ss, value)) {
            key = trim(key);
            value = trim(value);

            if (key == "name") profile.setName(value);
            else if (key == "age") {
                try { profile.setAge(std::stoi(value)); }
                catch (const std::invalid_argument& ia) { std::cerr << "Warning: Invalid age format in file: " << value << std::endl; }
                catch (const std::out_of_range& oor) { std::cerr << "Warning: Age out of range in file: " << value << std::endl; }
            }
            else if (key == "gender") profile.setGender(value);
            else if (key == "weight_lb") {
                try { profile.setWeightLb(std::stod(value)); }
                catch (const std::invalid_argument& ia) { std::cerr << "Warning: Invalid weight format in file: " << value << std::endl; }
                catch (const std::out_of_range& oor) { std::cerr << "Warning: Weight out of range in file: " << value << std::endl; }
            }
            else if (key == "height_ft_in") profile.setHeightFtIn(value);
            else if (key == "health_conditions") profile.setHealthConditions(value);
            else if (key == "health_goal") profile.setHealthGoal(value);
            else if (key == "preferred_exercise_duration_mins") {
                try { profile.setPreferredExerciseDurationMins(std::stoi(value)); }
                catch (const std::invalid_argument& ia) { std::cerr << "Warning: Invalid duration format in file: " << value << std::endl; }
                catch (const std::out_of_range& oor) { std::cerr << "Warning: Duration out of range in file: " << value << std::endl; }
            }
        } else {
            std::cerr << "Warning: Malformed line in profile file: " << line << std::endl;
        }
    }

    if (inFile.bad()) { // Serious I/O error
        std::cerr << "Error: A critical error occurred while reading file: " << filename << std::endl;
        inFile.close();
        return false;
    }

    inFile.close();
    return true;
}

} // namespace FileManager
