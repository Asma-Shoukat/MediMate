// File: core_logic/managers/historymanager.cpp
#include "historymanager.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

HistoryManager::HistoryManager(const std::string& directory)
    : historyDirectory(directory), maxEntriesPerSession(500) {
    try {
        if (!fs::exists(historyDirectory)) {
            fs::create_directories(historyDirectory);
            std::cout << "Info: History directory created at ./" << historyDirectory << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating history directory: " << e.what() << std::endl;
    }
}

std::string HistoryManager::getHistoryFilePath(const std::string& profileName) const {
    std::string safeName = profileName;
    std::replace(safeName.begin(), safeName.end(), ' ', '_');
    safeName.erase(std::remove_if(safeName.begin(), safeName.end(),
                                   [](char c) { return !std::isalnum(c) && c != '_'; }),
                   safeName.end());
    if (safeName.empty()) safeName = "default_history";
    return historyDirectory + "chat_" + safeName + ".txt";
}

void HistoryManager::startNewSession() {
    currentSessionHistory.clear();
}

void HistoryManager::addEntry(const ConversationEntry& entry) {
    if (static_cast<int>(currentSessionHistory.size()) >= maxEntriesPerSession) {
        // Remove oldest entries to keep within limits
        currentSessionHistory.erase(currentSessionHistory.begin());
    }
    currentSessionHistory.push_back(entry);
}

void HistoryManager::addEntry(SenderRole role, const std::string& senderName,
                               const std::string& message) {
    ConversationEntry entry(role, senderName, message);
    addEntry(entry);
}

std::vector<ConversationEntry> HistoryManager::getCurrentSessionHistory() const {
    return currentSessionHistory;
}

int HistoryManager::getCurrentSessionSize() const {
    return static_cast<int>(currentSessionHistory.size());
}

std::vector<ConversationEntry> HistoryManager::getRecentEntries(int count) const {
    std::vector<ConversationEntry> recent;
    int startIdx = std::max(0, static_cast<int>(currentSessionHistory.size()) - count);
    for (int i = startIdx; i < static_cast<int>(currentSessionHistory.size()); ++i) {
        recent.push_back(currentSessionHistory[i]);
    }
    return recent;
}

std::vector<ConversationEntry> HistoryManager::searchBySymptom(const std::string& symptom) const {
    std::vector<ConversationEntry> results;
    std::string lowerSymptom = symptom;
    std::transform(lowerSymptom.begin(), lowerSymptom.end(), lowerSymptom.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    for (const auto& entry : currentSessionHistory) {
        for (const auto& tag : entry.getSymptomTags()) {
            std::string lowerTag = tag;
            std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            if (lowerTag.find(lowerSymptom) != std::string::npos) {
                results.push_back(entry);
                break; // Only add once per entry
            }
        }
    }
    return results;
}

bool HistoryManager::saveSessionHistory(const std::string& profileName) const {
    if (currentSessionHistory.empty()) {
        std::cout << "Info: No history to save for " << profileName << std::endl;
        return true;
    }

    std::string filePath = getHistoryFilePath(profileName);
    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open history file for writing: " << filePath << std::endl;
        return false;
    }

    outFile << "SESSION_START\n";
    for (const auto& entry : currentSessionHistory) {
        outFile << entry.serialize();
    }
    outFile << "SESSION_END\n";

    if (outFile.fail()) {
        std::cerr << "Error: Failed to write history data to: " << filePath << std::endl;
        outFile.close();
        return false;
    }

    outFile.close();
    std::cout << "Info: Chat history saved for " << profileName << std::endl;
    return true;
}

namespace {
std::string trim_hist(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (std::string::npos == start) return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}
}

bool HistoryManager::loadSessionHistory(const std::string& profileName) {
    std::string filePath = getHistoryFilePath(profileName);
    std::ifstream inFile(filePath);
    if (!inFile.is_open()) {
        // Not an error — file may not exist yet
        return false;
    }

    currentSessionHistory.clear();
    std::string line;
    bool inEntry = false;
    ConversationEntry currentEntry;

    while (std::getline(inFile, line)) {
        line = trim_hist(line);
        if (line.empty() || line == "SESSION_START" || line == "SESSION_END") {
            continue;
        }

        if (line == "ENTRY_BEGIN") {
            inEntry = true;
            currentEntry = ConversationEntry();
            continue;
        }

        if (line == "ENTRY_END") {
            if (inEntry) {
                currentSessionHistory.push_back(currentEntry);
            }
            inEntry = false;
            continue;
        }

        if (inEntry) {
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) continue;
            std::string key = trim_hist(line.substr(0, colonPos));
            std::string value = trim_hist(line.substr(colonPos + 1));

            if (key == "timestamp")      currentEntry.setTimestamp(value);
            else if (key == "role")      currentEntry.setSenderRole(ConversationEntry::roleFromString(value));
            else if (key == "sender")    currentEntry.setSenderName(value);
            else if (key == "message")   currentEntry.setMessage(value);
            else if (key == "severity")  currentEntry.setSeverityLevel(value);
            else if (key == "symptom_tag") currentEntry.addSymptomTag(value);
        }
    }

    inFile.close();
    std::cout << "Info: Loaded " << currentSessionHistory.size() << " history entries for " << profileName << std::endl;
    return true;
}

void HistoryManager::clearCurrentSession() {
    currentSessionHistory.clear();
}
