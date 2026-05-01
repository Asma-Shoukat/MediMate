// File: core_logic/managers/profilemanager.cpp
#include "profilemanager.h"
#include "../persistence/file_manager.h" // We'll use our existing FileManager save/load logic

#include <fstream>      // For std::ifstream, std::ofstream
#include <iostream>     // For std::cerr (error reporting)
#include <filesystem>   // For directory listing (C++17)
#include <algorithm>    // For std::remove

// Namespace for filesystem might be std::filesystem or std::experimental::filesystem
// For C++17, it's std::filesystem
namespace fs = std::filesystem;

ProfileManager::ProfileManager(const std::string& directory) : profilesDirectory(directory) {
    // Ensure the profiles directory exists, create if not
    try {
        if (!fs::exists(profilesDirectory)) {
            fs::create_directories(profilesDirectory);
            std::cout << "Info: Profiles directory created at ./" << profilesDirectory << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating profiles directory: " << e.what() << std::endl;
        // Potentially throw or handle this more gracefully depending on application needs
    }
}

std::string ProfileManager::getProfilePath(const std::string& profileName) const {
    // Basic sanitization for profileName to be a valid filename part (very basic)
    // In a real app, more robust sanitization or using a separate safe "username" for filename is better.
    std::string safeProfileName = profileName;
    // Replace spaces with underscores, remove other problematic chars (simplified)
    std::replace(safeProfileName.begin(), safeProfileName.end(), ' ', '_');
    // Note: This doesn't handle all special characters.

    return profilesDirectory + safeProfileName + ".txt";
}

std::vector<std::string> ProfileManager::listProfileNames() const {
    std::vector<std::string> names;
    try {
        if (!fs::exists(profilesDirectory) || !fs::is_directory(profilesDirectory)) {
            std::cerr << "Error: Profiles directory '" << profilesDirectory << "' does not exist or is not a directory." << std::endl;
            return names;
        }
        for (const auto& entry : fs::directory_iterator(profilesDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string filename = entry.path().stem().string(); // Gets filename without extension
                // De-sanitize if needed (e.g., replace underscores back to spaces if that was the convention)
                std::replace(filename.begin(), filename.end(), '_', ' ');
                names.push_back(filename);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing profiles: " << e.what() << std::endl;
    }
    return names;
}

bool ProfileManager::loadProfile(const std::string& profileName) {
    if (profileName.empty()) {
        std::cerr << "Error: Profile name cannot be empty for loading." << std::endl;
        return false;
    }
    std::string filePath = getProfilePath(profileName);
    activeUserProfile = std::make_unique<UserProfile>(); // Create a new UserProfile instance

    if (FileManager::loadUserProfile(*activeUserProfile, filePath)) {
        // Ensure the loaded profile's internal name matches, or set it if it wasn't part of the file.
        // Our UserProfile currently gets its name from the file content.
        // If profileName is just for filename and UserProfile::name is from file data, this is fine.
        // If UserProfile::name should BE profileName, then:
        // activeUserProfile->setName(profileName); // But only if name isn't already in the file.
        // Your FileManager::loadUserProfile already populates the name field from the file.
        std::cout << "Info: Profile '" << activeUserProfile->getName() << "' loaded successfully." << std::endl;
        return true;
    } else {
        activeUserProfile.reset(); // Clear the profile if loading failed
        // FileManager::loadUserProfile might already print an info message if file not found
        // std::cerr << "Error: Could not load profile '" << profileName << "' from " << filePath << std::endl;
        return false;
    }
}

bool ProfileManager::saveCurrentProfile() const {
    if (!activeUserProfile) {
        std::cerr << "Error: No active profile to save." << std::endl;
        return false;
    }
    if (activeUserProfile->getName().empty()) {
        std::cerr << "Error: Active profile has no name, cannot determine filename to save." << std::endl;
        return false;
    }
    std::string filePath = getProfilePath(activeUserProfile->getName());
    if (FileManager::saveUserProfile(*activeUserProfile, filePath)) {
        std::cout << "Info: Profile '" << activeUserProfile->getName() << "' saved successfully to " << filePath << std::endl;
        return true;
    } else {
        std::cerr << "Error: Could not save profile '" << activeUserProfile->getName() << "' to " << filePath << std::endl;
        return false;
    }
}

void ProfileManager::createNewActiveProfile() {
    activeUserProfile = std::make_unique<UserProfile>();
    // The name and other details will be set via UserProfile setters in main.cpp
    // and then saveCurrentProfile() will use that name.
}

UserProfile* ProfileManager::getActiveProfile() const {
    return activeUserProfile.get(); // .get() returns the raw pointer from unique_ptr
}

void ProfileManager::clearActiveProfile() {
    activeUserProfile.reset();
}

bool ProfileManager::profileExists(const std::string& profileName) const {
    if (profileName.empty()) return false;
    return fs::exists(getProfilePath(profileName));
}

bool ProfileManager::deleteProfile(const std::string& profileName) {
    if (profileName.empty()) return false;
    std::string filePath = getProfilePath(profileName);
    try {
        if (fs::exists(filePath)) {
            fs::remove(filePath);
            std::cout << "Info: Profile '" << profileName << "' deleted." << std::endl;
            // If the deleted profile is the active one, clear it
            if (activeUserProfile && activeUserProfile->getName() == profileName) {
                activeUserProfile.reset();
            }
            return true;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error deleting profile: " << e.what() << std::endl;
    }
    return false;
}
