// File: core_logic/managers/profilemanager.h
#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "../models/userprofile.h" // For UserProfile class
#include <string>
#include <vector>
#include <memory> // For std::unique_ptr to manage UserProfile potentially

class ProfileManager {
private:
    std::string profilesDirectory;    // Directory where profile files are stored
    std::unique_ptr<UserProfile> activeUserProfile; // The currently loaded profile

    // Helper to construct the full path for a profile file
    std::string getProfilePath(const std::string& profileName) const;

public:
    // Constructor - takes the directory where profiles are stored
    explicit ProfileManager(const std::string& directory = "profiles/");

    // Lists all available profile names (e.g., from filenames in the directory)
    std::vector<std::string> listProfileNames() const;

    // Loads a profile by name into activeUserProfile.
    // Returns true on success, false if profile not found or error.
    bool loadProfile(const std::string& profileName);

    // Saves the currently activeUserProfile.
    // If profileName is provided and different from active profile's name,
    // it effectively saves as a new profile or overwrites another.
    // Returns true on success, false on failure.
    // For a new profile, activeUserProfile should be populated first.
    bool saveCurrentProfile() const;

    // Creates a new, empty UserProfile and sets it as active.
    // The caller will then populate it and call saveCurrentProfile.
    void createNewActiveProfile();

    // Returns a pointer to the currently active user profile.
    // Returns nullptr if no profile is active.
    // The ProfileManager owns the UserProfile object.
    UserProfile* getActiveProfile() const;

    // Clears the active profile
    void clearActiveProfile();

    // Checks if a profile with the given name exists
    bool profileExists(const std::string& profileName) const;

    // Deletes a profile file permanently
    bool deleteProfile(const std::string& profileName);

    // Returns the directory path used
    std::string getDirectory() const { return profilesDirectory; }
};

#endif // PROFILEMANAGER_H
