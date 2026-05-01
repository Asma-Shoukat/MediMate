// File: core_logic/persistence/file_manager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "../models/userprofile.h" // Adjust path as per your project structure
#include <string>

namespace FileManager { // Using a namespace to group these utility functions

// Saves the user profile to a file.
// Returns true on success, false on failure.
bool saveUserProfile(const UserProfile& profile, const std::string& filename);

// Loads the user profile from a file.
// Modifies the 'profile' object passed by reference.
// Returns true on success, false on failure (e.g., file not found, format error).
bool loadUserProfile(UserProfile& profile, const std::string& filename);

} // namespace FileManager

#endif // FILE_MANAGER_H
