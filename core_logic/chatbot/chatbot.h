// File: core_logic/chatbot/chatbot.h — COMPLETE REWRITE: Smart Conversational Health Engine
#ifndef CHATBOT_H
#define CHATBOT_H

#include <string>
#include <vector>
#include <random>
#include <map>
#include <set>

class UserProfile;
class ToDoManager;
class HistoryManager;

// Expanded conversation states for multi-turn symptom analysis
enum class ConversationState {
    IDLE,                       // Default state, waiting for user input
    SYMPTOM_GATHERING,          // Actively collecting symptoms
    SEVERITY_ASSESSMENT,        // Asking about severity/duration
    PROVIDING_DIAGNOSIS,        // Generating structured health response
    AWAITING_RECOMMEND_TYPE,    // Waiting for diet/exercise choice
    EMERGENCY_DETECTED          // Urgent condition flagged
};

using DietPlan = std::vector<std::string>;

// Structured symptom knowledge entry
struct SymptomEntry {
    std::string symptomName;
    std::vector<std::string> possibleCauses;
    std::string defaultSeverity;          // "low", "medium", "high"
    std::vector<std::string> followUpQuestions;
    std::vector<std::string> recommendedActions;
    std::vector<std::string> redFlags;    // Dangerous accompanying symptoms
    bool isEmergency;
};

// Structured diagnosis response
struct DiagnosisResponse {
    std::vector<std::string> detectedSymptoms;
    std::vector<std::string> possibleCauses;
    std::string severityLevel;            // "low", "medium", "high", "emergency"
    std::vector<std::string> suggestedActions;
    std::string disclaimer;
    bool isEmergency;
};

class ChatBot {
private:
    UserProfile* currentUserProfile;
    ToDoManager* userToDoManager;
    HistoryManager* historyManager;

    // Conversation context
    ConversationState currentConvState;
    std::vector<std::string> gatheredSymptoms;
    std::vector<std::string> askedQuestions;
    int currentFollowUpIndex;
    std::string currentPrimarySymptom;
    int reportedSeverity;  // 1-10 scale from user or auto-assessed

    // Response pools
    std::vector<std::string> greetings;
    std::vector<std::string> farewells;
    std::vector<std::string> encouragements;
    std::mt19937 randomGenerator;
    DietPlan lastGeneratedDietPlan;

    // Symptom Knowledge Base
    std::map<std::string, SymptomEntry> symptomDatabase;
    std::vector<std::string> emergencyKeywords;

    // ---- Private methods ----

    // Initialization
    void initializeSymptomDatabase();
    void initializeEmergencyKeywords();
    void initializeResponsePools();

    // Symptom analysis pipeline
    std::vector<std::string> extractSymptoms(const std::string& input) const;
    bool checkForEmergency(const std::string& input) const;
    DiagnosisResponse generateDiagnosis() const;
    std::string formatDiagnosisResponse(const DiagnosisResponse& diagnosis) const;
    std::string generateFollowUpQuestion() const;
    std::string assessSeverityFromContext() const;

    // State handlers
    std::string handleSymptomInput(const std::string& input);
    std::string handleSeverityResponse(const std::string& input);
    std::string handleEmergency(const std::string& input);

    // Original command handlers (enhanced)
    std::string handleGreeting();
    std::string handleFarewell();
    std::string handleViewProfile();
    std::string handleRecommendDiet();
    std::string handleRecommendExercise();
    std::string handleAddCustomTask(const std::string& originalTitle);
    std::string handleViewTasks();
    std::string handleCompleteTask(const std::vector<std::string>& tokens);
    std::string handleHelp();
    std::string handleSummary();
    std::string handleHealthScore();
    std::string handleViewHistory();
    std::string handleUnknownCommand(const std::string& originalCommandWord = "");

    // Utilities
    std::string getRandomResponse(const std::vector<std::string>& responses);
    std::vector<std::string> tokenize(const std::string& input) const;
    std::string getPersonalizedGreeting() const;
    std::string getMedicalDisclaimer() const;
    bool isSymptomRelatedInput(const std::string& input) const;

public:
    ChatBot(UserProfile* profile, ToDoManager* todoManager,
            HistoryManager* histMgr = nullptr);
    std::string processInput(const std::string& userInput);
    const DietPlan& getLastGeneratedDietPlan() const;
    void clearLastDietPlan();

    // New public getters for UI
    ConversationState getCurrentState() const;
    std::vector<std::string> getGatheredSymptoms() const;
    int getReportedSeverity() const;
    std::vector<std::string> getSuggestionChips() const;
};

#endif // CHATBOT_H
