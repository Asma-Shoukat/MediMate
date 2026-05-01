// File: core_logic/managers/historymanager.h
#ifndef HISTORYMANAGER_H
#define HISTORYMANAGER_H

#include "../models/conversationentry.h"
#include <string>
#include <vector>

class HistoryManager {
private:
    std::vector<ConversationEntry> currentSessionHistory;
    std::string historyDirectory;
    int maxEntriesPerSession;

    std::string getHistoryFilePath(const std::string& profileName) const;

public:
    explicit HistoryManager(const std::string& directory = "history/");

    // Session management
    void startNewSession();
    void addEntry(const ConversationEntry& entry);
    void addEntry(SenderRole role, const std::string& senderName,
                  const std::string& message);

    // Getters
    std::vector<ConversationEntry> getCurrentSessionHistory() const;
    int getCurrentSessionSize() const;

    // Get recent entries (last N)
    std::vector<ConversationEntry> getRecentEntries(int count) const;

    // Search history by symptom tags
    std::vector<ConversationEntry> searchBySymptom(const std::string& symptom) const;

    // Persistence
    bool saveSessionHistory(const std::string& profileName) const;
    bool loadSessionHistory(const std::string& profileName);

    // Clear
    void clearCurrentSession();
};

#endif // HISTORYMANAGER_H
