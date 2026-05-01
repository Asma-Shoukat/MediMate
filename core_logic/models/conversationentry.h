// File: core_logic/models/conversationentry.h
#ifndef CONVERSATIONENTRY_H
#define CONVERSATIONENTRY_H

#include <string>
#include <vector>
#include <ctime>

enum class SenderRole {
    USER,
    BOT,
    SYSTEM,
    EMERGENCY
};

class ConversationEntry {
private:
    std::string timestamp;       // ISO-style timestamp string
    SenderRole senderRole;
    std::string senderName;
    std::string message;
    std::vector<std::string> symptomTags;  // Symptoms mentioned in this entry
    std::string severityLevel;   // "low", "medium", "high", "emergency", or empty

public:
    // Constructors
    ConversationEntry();
    ConversationEntry(SenderRole role, const std::string& senderName,
                      const std::string& message);
    ConversationEntry(const std::string& timestamp, SenderRole role,
                      const std::string& senderName, const std::string& message);

    // Getters
    std::string getTimestamp() const;
    SenderRole getSenderRole() const;
    std::string getSenderRoleString() const;
    std::string getSenderName() const;
    std::string getMessage() const;
    std::vector<std::string> getSymptomTags() const;
    std::string getSeverityLevel() const;

    // Setters
    void setTimestamp(const std::string& timestamp);
    void setSenderRole(SenderRole role);
    void setSenderName(const std::string& name);
    void setMessage(const std::string& message);
    void addSymptomTag(const std::string& symptom);
    void setSymptomTags(const std::vector<std::string>& tags);
    void setSeverityLevel(const std::string& level);

    // Utility
    static std::string generateTimestamp();
    static SenderRole roleFromString(const std::string& roleStr);

    // Serialization for file persistence
    std::string serialize() const;
};

#endif // CONVERSATIONENTRY_H
