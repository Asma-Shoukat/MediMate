// File: core_logic/models/conversationentry.cpp
#include "conversationentry.h"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

// Default Constructor
ConversationEntry::ConversationEntry()
    : senderRole(SenderRole::SYSTEM) {
    timestamp = generateTimestamp();
}

// Constructor with auto-timestamp
ConversationEntry::ConversationEntry(SenderRole role, const std::string& senderName,
                                     const std::string& message)
    : senderRole(role), senderName(senderName), message(message) {
    timestamp = generateTimestamp();
}

// Constructor with explicit timestamp (for loading from file)
ConversationEntry::ConversationEntry(const std::string& timestamp, SenderRole role,
                                     const std::string& senderName, const std::string& message)
    : timestamp(timestamp), senderRole(role), senderName(senderName), message(message) {
}

// Getters
std::string ConversationEntry::getTimestamp() const { return timestamp; }
SenderRole ConversationEntry::getSenderRole() const { return senderRole; }

std::string ConversationEntry::getSenderRoleString() const {
    switch (senderRole) {
        case SenderRole::USER:      return "USER";
        case SenderRole::BOT:       return "BOT";
        case SenderRole::SYSTEM:    return "SYSTEM";
        case SenderRole::EMERGENCY: return "EMERGENCY";
        default:                    return "UNKNOWN";
    }
}

std::string ConversationEntry::getSenderName() const { return senderName; }
std::string ConversationEntry::getMessage() const { return message; }
std::vector<std::string> ConversationEntry::getSymptomTags() const { return symptomTags; }
std::string ConversationEntry::getSeverityLevel() const { return severityLevel; }

// Setters
void ConversationEntry::setTimestamp(const std::string& ts) { timestamp = ts; }
void ConversationEntry::setSenderRole(SenderRole role) { senderRole = role; }
void ConversationEntry::setSenderName(const std::string& name) { senderName = name; }
void ConversationEntry::setMessage(const std::string& msg) { message = msg; }

void ConversationEntry::addSymptomTag(const std::string& symptom) {
    symptomTags.push_back(symptom);
}

void ConversationEntry::setSymptomTags(const std::vector<std::string>& tags) {
    symptomTags = tags;
}

void ConversationEntry::setSeverityLevel(const std::string& level) {
    severityLevel = level;
}

// Static Utilities
std::string ConversationEntry::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &time_t_now);
#else
    localtime_r(&time_t_now, &local_tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

SenderRole ConversationEntry::roleFromString(const std::string& roleStr) {
    if (roleStr == "USER")      return SenderRole::USER;
    if (roleStr == "BOT")       return SenderRole::BOT;
    if (roleStr == "EMERGENCY") return SenderRole::EMERGENCY;
    return SenderRole::SYSTEM;
}

// Serialization: key:value format for file persistence
std::string ConversationEntry::serialize() const {
    std::ostringstream oss;
    oss << "ENTRY_BEGIN\n";
    oss << "timestamp:" << timestamp << "\n";
    oss << "role:" << getSenderRoleString() << "\n";
    oss << "sender:" << senderName << "\n";
    oss << "message:" << message << "\n";
    if (!severityLevel.empty()) {
        oss << "severity:" << severityLevel << "\n";
    }
    for (const auto& tag : symptomTags) {
        oss << "symptom_tag:" << tag << "\n";
    }
    oss << "ENTRY_END\n";
    return oss.str();
}
