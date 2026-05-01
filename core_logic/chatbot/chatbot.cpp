// File: core_logic/chatbot/chatbot.cpp
#include "chatbot.h"
#include "../models/userprofile.h"
#include "../managers/todomanager.h"
#include "../managers/historymanager.h"
#include "../models/exercisetask.h"
#include "../models/diettask.h"
#include "../models/generictask.h"
#include "../models/healthscore.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <tuple>
#include <stdexcept>

ChatBot::ChatBot(UserProfile* profile, ToDoManager* todoManager, HistoryManager* histMgr)
    : currentUserProfile(profile),
      userToDoManager(todoManager),
      historyManager(histMgr),
      currentConvState(ConversationState::IDLE),
      currentFollowUpIndex(0),
      reportedSeverity(0),
      randomGenerator(std::random_device{}()) {
    
    if (!currentUserProfile || !userToDoManager) {
        throw std::runtime_error("ChatBot constructor requires valid UserProfile and ToDoManager.");
    }

    initializeSymptomDatabase();
    initializeEmergencyKeywords();
    initializeResponsePools();
}

void ChatBot::initializeSymptomDatabase() {
    // 1. Headache
    SymptomEntry headache;
    headache.symptomName = "headache";
    headache.possibleCauses = {"Tension", "Dehydration", "Migraine", "Stress"};
    headache.defaultSeverity = "low";
    headache.followUpQuestions = {"How long have you had the headache?", "Is the pain throbbing, dull, or sharp?"};
    headache.recommendedActions = {"Drink plenty of water", "Rest in a quiet, dark room", "Take over-the-counter pain relief if safe for you"};
    headache.redFlags = {"Sudden, severe headache ('thunderclap')", "Headache with fever and stiff neck", "Headache after head injury"};
    headache.isEmergency = false;
    symptomDatabase["headache"] = headache;
    symptomDatabase["headaches"] = headache;
    symptomDatabase["migraine"] = headache;

    // 2. Fever
    SymptomEntry fever;
    fever.symptomName = "fever";
    fever.possibleCauses = {"Viral infection", "Bacterial infection", "Flu", "Common cold"};
    fever.defaultSeverity = "medium";
    fever.followUpQuestions = {"What is your temperature?", "Do you have chills or body aches?"};
    fever.recommendedActions = {"Rest and stay hydrated", "Take fever-reducing medication like acetaminophen", "Monitor temperature"};
    fever.redFlags = {"Fever over 103°F (39.4°C)", "Fever lasting more than 3 days", "Fever with severe headache and stiff neck"};
    fever.isEmergency = false;
    symptomDatabase["fever"] = fever;
    symptomDatabase["temperature"] = fever;

    // 3. Cough
    SymptomEntry cough;
    cough.symptomName = "cough";
    cough.possibleCauses = {"Common cold", "Flu", "Allergies", "Bronchitis"};
    cough.defaultSeverity = "low";
    cough.followUpQuestions = {"Is the cough dry or productive (producing phlegm)?", "How long have you been coughing?"};
    cough.recommendedActions = {"Drink warm fluids", "Use a humidifier", "Try throat lozenges"};
    cough.redFlags = {"Coughing up blood", "Difficulty breathing", "Cough lasting more than 3 weeks"};
    cough.isEmergency = false;
    symptomDatabase["cough"] = cough;
    symptomDatabase["coughing"] = cough;

    // 4. Chest Pain (High severity/Emergency potential)
    SymptomEntry chestPain;
    chestPain.symptomName = "chest pain";
    chestPain.possibleCauses = {"Heart attack", "Angina", "Heartburn/GERD", "Muscle strain", "Panic attack"};
    chestPain.defaultSeverity = "high";
    chestPain.followUpQuestions = {"Does the pain radiate to your arm, neck, or jaw?", "Are you experiencing shortness of breath or sweating?"};
    chestPain.recommendedActions = {"SEEK IMMEDIATE MEDICAL ATTENTION if pain is severe, crushing, or radiating.", "Rest immediately."};
    chestPain.redFlags = {"Crushing chest pain", "Pain radiating to left arm/jaw", "Shortness of breath", "Sweating/nausea"};
    chestPain.isEmergency = true;
    symptomDatabase["chest pain"] = chestPain;
    symptomDatabase["heart hurts"] = chestPain;

    // 5. Nausea/Vomiting
    SymptomEntry nausea;
    nausea.symptomName = "nausea";
    nausea.possibleCauses = {"Food poisoning", "Stomach flu", "Motion sickness", "Pregnancy"};
    nausea.defaultSeverity = "medium";
    nausea.followUpQuestions = {"Have you been able to keep any fluids down?", "Are you experiencing diarrhea as well?"};
    nausea.recommendedActions = {"Sip clear fluids (water, broth, sports drinks)", "Eat bland foods (BRAT diet: bananas, rice, applesauce, toast)", "Avoid strong odors"};
    nausea.redFlags = {"Inability to keep fluids down for 24 hours", "Vomiting blood", "Signs of severe dehydration (dry mouth, dark urine, dizziness)"};
    nausea.isEmergency = false;
    symptomDatabase["nausea"] = nausea;
    symptomDatabase["vomiting"] = nausea;
    symptomDatabase["throw up"] = nausea;
    symptomDatabase["stomach ache"] = nausea;

    // Add more as needed...
}

void ChatBot::initializeEmergencyKeywords() {
    emergencyKeywords = {
        "chest pain", "heart attack", "can't breathe", "cannot breathe", "shortness of breath",
        "bleeding heavily", "severe bleeding", "stroke", "face drooping", "arm weakness",
        "slurred speech", "suicide", "kill myself", "want to die", "unconscious",
        "fainted", "passed out", "seizure", "choking"
    };
}

void ChatBot::initializeResponsePools() {
    greetings = {
        "Hi there! 👋 How can I support you today?",
        "Hello! 😊 How are you feeling right now?",
        "Welcome back! 🌿 What's on your mind health-wise today?",
        "Hey! 💙 I'm here and ready to help. What's going on?",
        "Good to see you! ✨ Tell me — how are you feeling today?",
        "Hi! Your health matters to me. 🙏 What can I help you with?"
    };
    farewells = {
        "Take care of yourself! 💙 Feel better soon.",
        "Goodbye! Don't forget to stay hydrated. 💧",
        "Bye for now! I'm here whenever you need me. ✨",
        "Take it easy! Rest up and reach out anytime. 🌙",
        "See you soon! Remember — small healthy habits add up. 🌿",
        "Goodbye! Stay well and don't hesitate to come back. 😊"
    };
    encouragements = {
        "You're taking a wonderful step by tracking your health. 🌟",
        "I'm here to help you understand your symptoms. You're not alone! 🤗",
        "Great job being proactive about your well-being. 💪",
        "Every check-in is a step in the right direction. Keep going! 🏃",
        "Your health journey matters, and I'm proud of you for checking in. 💚"
    };
}

std::vector<std::string> ChatBot::tokenize(const std::string& input) const {
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (ss >> token) {
        std::string t = token;
        std::transform(t.begin(), t.end(), t.begin(), [](unsigned char c){ return std::tolower(c); });
        // Remove common punctuation from ends
        while(!t.empty() && ispunct(t.back())) t.pop_back();
        while(!t.empty() && ispunct(t.front())) t.erase(t.begin());
        if (!t.empty()) tokens.push_back(t);
    }
    return tokens;
}

std::string ChatBot::getMedicalDisclaimer() const {
    return "<i><b>Disclaimer:</b> I am an AI, not a doctor. This information is for educational purposes only. Always consult a healthcare professional for medical advice, diagnosis, or treatment.</i>";
}

bool ChatBot::checkForEmergency(const std::string& input) const {
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    for (const auto& kw : emergencyKeywords) {
        if (lowerInput.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ChatBot::extractSymptoms(const std::string& input) const {
    std::vector<std::string> found;
    std::string lowerInput = input;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    
    for (const auto& pair : symptomDatabase) {
        if (lowerInput.find(pair.first) != std::string::npos) {
            // Check if it's already added (via another synonym)
            bool alreadyAdded = false;
            for(const auto& f : found) {
                if(symptomDatabase.at(f).symptomName == pair.second.symptomName) {
                    alreadyAdded = true; break;
                }
            }
            if(!alreadyAdded) {
                found.push_back(pair.first);
            }
        }
    }
    return found;
}

std::string ChatBot::handleEmergency(const std::string& input) {
    (void)input;
    currentConvState = ConversationState::IDLE;
    gatheredSymptoms.clear();
    
    if (historyManager) {
        historyManager->addEntry(SenderRole::EMERGENCY, "Bot", "EMERGENCY DETECTED");
    }

    std::ostringstream oss;
    oss << "<b style='color:red;'>🚨 EMERGENCY WARNING 🚨</b><br><br>";
    oss << "Your message indicates a potentially life-threatening condition.<br><br>";
    oss << "<b>PLEASE STOP CHATTING AND SEEK IMMEDIATE EMERGENCY MEDICAL HELP.</b><br>";
    oss << "Call 911 (or your local emergency number) or go to the nearest emergency room immediately.<br>";
    return oss.str();
}

std::string ChatBot::getRandomResponse(const std::vector<std::string>& responses) {
    if (responses.empty()) return "";
    std::uniform_int_distribution<> dist(0, responses.size() - 1);
    return responses[dist(randomGenerator)];
}

std::string ChatBot::processInput(const std::string& userInput) {
    if (userInput.empty()) return "Please say something.";

    if (historyManager) {
        ConversationEntry userEntry(SenderRole::USER, currentUserProfile->getName(), userInput);
        historyManager->addEntry(userEntry);
    }

    if (checkForEmergency(userInput)) {
        return handleEmergency(userInput);
    }

    std::string lowerInput = userInput;
    std::transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
    std::vector<std::string> tokens = tokenize(userInput);
    if (tokens.empty()) return "I received an empty command.";
    
    const std::string& commandWord = tokens[0];

    // State Machine routing
    if (currentConvState == ConversationState::AWAITING_RECOMMEND_TYPE) {
        if (commandWord == "diet" || commandWord == "meal" || commandWord == "food") { 
            currentConvState = ConversationState::IDLE; return handleRecommendDiet(); 
        } else if (commandWord == "exercise" || commandWord == "workout" || commandWord == "activity") { 
            currentConvState = ConversationState::IDLE; clearLastDietPlan(); return handleRecommendExercise(); 
        } else if (commandWord == "cancel" || commandWord == "stop") { 
            currentConvState = ConversationState::IDLE; clearLastDietPlan(); return "Recommendation cancelled."; 
        } else { 
            return "Please specify 'diet' or 'exercise', or 'cancel'."; 
        }
    } else if (currentConvState == ConversationState::SYMPTOM_GATHERING) {
        return handleSymptomInput(userInput);
    } else if (currentConvState == ConversationState::SEVERITY_ASSESSMENT) {
        return handleSeverityResponse(userInput);
    }

    // Command routing (IDLE state)
    if (lowerInput.find("hello") != std::string::npos || lowerInput.find("hi") != std::string::npos) return handleGreeting();
    if (lowerInput.find("bye") != std::string::npos || lowerInput.find("exit") != std::string::npos) return handleFarewell();
    if (lowerInput.find("profile") != std::string::npos) return handleViewProfile();
    if (lowerInput.find("tasks") != std::string::npos || lowerInput.find("todolist") != std::string::npos) return handleViewTasks();
    if (lowerInput.find("help") != std::string::npos) return handleHelp();
    if (lowerInput.find("summary") != std::string::npos) return handleSummary();
    if (lowerInput.find("health score") != std::string::npos || lowerInput.find("score") != std::string::npos) return handleHealthScore();
    
    if (lowerInput.find("recommend diet") != std::string::npos) { currentConvState = ConversationState::IDLE; return handleRecommendDiet(); }
    if (lowerInput.find("recommend exercise") != std::string::npos) { currentConvState = ConversationState::IDLE; return handleRecommendExercise(); }
    
    if (lowerInput.find("recommend") != std::string::npos) {
        currentConvState = ConversationState::AWAITING_RECOMMEND_TYPE;
        return "Are you looking for a 'diet' or 'exercise' recommendation?";
    }
    
    if (commandWord == "addtask" && tokens.size() > 1) {
        size_t spacePos = userInput.find_first_of(" \t");
        if (spacePos != std::string::npos) {
            return handleAddCustomTask(userInput.substr(spacePos + 1));
        }
    }
    
    if (commandWord == "complete" && tokens.size() > 1) {
        return handleCompleteTask(tokens);
    }

    // If not a command, treat as potential symptom input
    std::vector<std::string> symptoms = extractSymptoms(userInput);
    if (!symptoms.empty()) {
        gatheredSymptoms = symptoms;
        currentPrimarySymptom = symptoms[0]; // Take first for follow-ups
        
        // Add tags to history
        if (historyManager) {
            auto entries = historyManager->getCurrentSessionHistory();
            if (!entries.empty()) {
                ConversationEntry& lastUserEntry = const_cast<ConversationEntry&>(entries.back());
                lastUserEntry.setSymptomTags(gatheredSymptoms);
            }
        }

        SymptomEntry& entry = symptomDatabase[currentPrimarySymptom];
        if (entry.isEmergency) {
             return handleEmergency(userInput);
        }

        currentConvState = ConversationState::SEVERITY_ASSESSMENT;
        return "I'm so sorry to hear you're experiencing <b>" + currentPrimarySymptom + "</b> 😕. To help me understand better, on a scale of 1 to 10, how severe is it right now?";
    }

    return handleUnknownCommand(commandWord);
}

std::string ChatBot::handleSymptomInput(const std::string& input) {
    // This handles additional symptoms if we were in a gathering state
    std::vector<std::string> moreSymptoms = extractSymptoms(input);
    for (const auto& s : moreSymptoms) {
        if (std::find(gatheredSymptoms.begin(), gatheredSymptoms.end(), s) == gatheredSymptoms.end()) {
            gatheredSymptoms.push_back(s);
        }
    }
    
    // Jump to diagnosis
    return formatDiagnosisResponse(generateDiagnosis());
}

std::string ChatBot::handleSeverityResponse(const std::string& input) {
    std::vector<std::string> tokens = tokenize(input);
    reportedSeverity = 0;
    
    for (const auto& t : tokens) {
        try {
            reportedSeverity = std::stoi(t);
            if (reportedSeverity >= 1 && reportedSeverity <= 10) break;
        } catch (...) { /* ignore non-numbers */ }
    }

    // If severe, we might jump straight to diagnosis
    if (reportedSeverity >= 8) {
        return formatDiagnosisResponse(generateDiagnosis());
    }

    // Otherwise, ask a follow-up question
    SymptomEntry& entry = symptomDatabase[currentPrimarySymptom];
    if (static_cast<size_t>(currentFollowUpIndex) < entry.followUpQuestions.size()) {
        std::string q = entry.followUpQuestions[currentFollowUpIndex++];
        currentConvState = ConversationState::SYMPTOM_GATHERING;
        return "Thanks for sharing that. " + q;
    }

    // No more questions, give diagnosis
    return formatDiagnosisResponse(generateDiagnosis());
}

DiagnosisResponse ChatBot::generateDiagnosis() const {
    DiagnosisResponse resp;
    resp.isEmergency = false;
    resp.detectedSymptoms = gatheredSymptoms;
    
    std::set<std::string> allCauses;
    std::set<std::string> allActions;
    int maxSeverityVal = reportedSeverity;

    for (const auto& symKey : gatheredSymptoms) {
        auto it = symptomDatabase.find(symKey);
        if (it != symptomDatabase.end()) {
            for (const auto& c : it->second.possibleCauses) allCauses.insert(c);
            for (const auto& a : it->second.recommendedActions) allActions.insert(a);
            
            // simple severity logic
            int sVal = 0;
            if (it->second.defaultSeverity == "high") sVal = 7;
            if (it->second.defaultSeverity == "medium") sVal = 4;
            if (it->second.defaultSeverity == "low") sVal = 2;
            if (sVal > maxSeverityVal) maxSeverityVal = sVal;
        }
    }

    resp.possibleCauses.assign(allCauses.begin(), allCauses.end());
    resp.suggestedActions.assign(allActions.begin(), allActions.end());
    
    if (maxSeverityVal >= 8) resp.severityLevel = "high";
    else if (maxSeverityVal >= 4) resp.severityLevel = "medium";
    else resp.severityLevel = "low";

    resp.disclaimer = getMedicalDisclaimer();
    return resp;
}

std::string ChatBot::formatDiagnosisResponse(const DiagnosisResponse& diagnosis) const {
    const_cast<ChatBot*>(this)->currentConvState = ConversationState::IDLE; // Reset state
    const_cast<ChatBot*>(this)->currentFollowUpIndex = 0;
    
    std::ostringstream oss;
    oss << "Thanks for giving me those details. Based on what you've shared (<b>";
    for (size_t i = 0; i < diagnosis.detectedSymptoms.size(); ++i) {
        oss << diagnosis.detectedSymptoms[i];
        if (i < diagnosis.detectedSymptoms.size() - 1) oss << ", ";
    }
    oss << "</b>), here is what I found:<br><br>";

    // Severity block
    std::string color = "#2563EB"; // Royal Blue for low
    std::string emoji = "🟢";
    if (diagnosis.severityLevel == "high") { color = "#DC2626"; emoji = "🔴"; } // Red for high
    else if (diagnosis.severityLevel == "medium") { color = "#D97706"; emoji = "🟡"; } // Amber for medium
    
    oss << "<b>" << emoji << " Estimated Severity:</b> <span style='color:" << color << "; font-weight:bold; text-transform: capitalize;'>" 
        << diagnosis.severityLevel << "</span><br><br>";

    // Causes
    oss << "<b>🔍 Possible Causes:</b><ul style='margin-top: 5px; margin-bottom: 15px;'>";
    for (size_t i = 0; i < std::min((size_t)4, diagnosis.possibleCauses.size()); ++i) {
         oss << "<li>" << diagnosis.possibleCauses[i] << "</li>";
    }
    oss << "</ul>";

    // Actions
    oss << "<b>💡 Suggested Actions:</b><ul style='margin-top: 5px; margin-bottom: 15px;'>";
    for (size_t i = 0; i < std::min((size_t)4, diagnosis.suggestedActions.size()); ++i) {
         oss << "<li>" << diagnosis.suggestedActions[i] << "</li>";
    }
    oss << "</ul>";

    // Task suggestion
    if (userToDoManager && !diagnosis.suggestedActions.empty()) {
        std::string taskTitle = diagnosis.suggestedActions[0];
        int id = userToDoManager->generateNewTaskID();
        userToDoManager->addTask(new GenericTask(id, taskTitle, "Suggested by symptom checker"));
        oss << "<i>I've added '" << taskTitle << "' to your tasks.</i><br><br>";
    }

    oss << diagnosis.disclaimer;
    
    // Severity-aware closing advice
    if (diagnosis.severityLevel == "high") {
        oss << "<br><br>⚠️ <b>Given the severity, please consider seeing a doctor today if symptoms persist or worsen.</b>";
    } else if (diagnosis.severityLevel == "medium") {
        oss << "<br><br>💬 Monitor your symptoms closely. If things don't improve in 24&#8211;48 hours, consult a healthcare provider.";
    } else {
        oss << "<br><br>💚 These symptoms appear mild. Rest well, stay hydrated, and check in with me if anything changes!";
    }
    
    const_cast<ChatBot*>(this)->gatheredSymptoms.clear();
    return oss.str();
}

// Basic Handlers
std::string ChatBot::handleGreeting() {
    std::string name = currentUserProfile ? currentUserProfile->getName() : "there";
    std::vector<std::string> personalGreetings = {
        "Hi <b>" + name + "</b>! 👋 How are you feeling today? Describe a symptom or ask for health advice.",
        "Hello <b>" + name + "</b>! 😊 I'm here to help. What's on your mind health-wise?",
        "Good to see you, <b>" + name + "</b>! 🌿 What health topic can I help you with today?",
        "Hey <b>" + name + "</b>! 💙 Ready to support your wellness. What's going on?",
        "Welcome back, <b>" + name + "</b>! ✨ Feeling well, or is something bothering you?"
    };
    return getRandomResponse(personalGreetings);
}
std::string ChatBot::handleFarewell() { return getRandomResponse(farewells); }

std::string ChatBot::handleViewProfile() { 
    if (!currentUserProfile) return "No active profile loaded.";
    const std::string& name = currentUserProfile->getName();
    std::ostringstream oss;
    oss << "📄 <b>Health Profile — " << name << "</b><br><br>";
    oss << "<b>Age:</b> " << currentUserProfile->getAge() << " years<br>";
    oss << "<b>Gender:</b> " << currentUserProfile->getGender() << "<br>";
    oss << "<b>Weight:</b> " << currentUserProfile->getWeightLb() << " lbs<br>";
    oss << "<b>Height:</b> " << currentUserProfile->getHeightFtIn() << "<br>";
    if (!currentUserProfile->getHealthConditions().empty())
        oss << "<b>Conditions:</b> " << currentUserProfile->getHealthConditions() << "<br>";
    oss << "<b>Health Goal:</b> " << currentUserProfile->getHealthGoal() << "<br><br>";
    oss << "<i>Your data is stored privately and locally. 🔒</i>";
    return oss.str();
}

std::string ChatBot::handleRecommendDiet() {
    clearLastDietPlan();
    if (!currentUserProfile) return "I need an active profile to recommend a diet plan.";
    
    std::string goal = currentUserProfile->getHealthGoal();
    std::string conditions = currentUserProfile->getHealthConditions();
    std::string name = currentUserProfile->getName();
    
    std::ostringstream oss;
    oss << "🍽️ <b>Personalised Diet Plan for " << name << "</b><br>";
    oss << "<i>Goal: " << goal << "</i><br><br>";
    
    oss << "<b>🌅 Breakfast:</b><ul>";
    if (goal.find("muscle") != std::string::npos || goal.find("build") != std::string::npos) {
        oss << "<li>3 scrambled eggs with spinach</li>";
        oss << "<li>Oatmeal with banana &amp; almond butter</li>";
        oss << "<li>Black coffee or green tea</li>";
    } else if (goal.find("weight") != std::string::npos || goal.find("lose") != std::string::npos) {
        oss << "<li>Greek yogurt with berries (low sugar)</li>";
        oss << "<li>1 boiled egg</li>";
        oss << "<li>Black coffee or herbal tea</li>";
    } else {
        oss << "<li>Whole grain toast with avocado</li>";
        oss << "<li>Fresh fruit &amp; a glass of water</li>";
        oss << "<li>Green tea</li>";
    }
    oss << "</ul>";
    
    oss << "<b>🌞 Lunch:</b><ul>";
    if (goal.find("muscle") != std::string::npos || goal.find("build") != std::string::npos) {
        oss << "<li>Grilled chicken breast or tuna (200g)</li>";
        oss << "<li>Brown rice &amp; steamed broccoli</li>";
        oss << "<li>Protein shake (optional)</li>";
    } else if (goal.find("weight") != std::string::npos || goal.find("lose") != std::string::npos) {
        oss << "<li>Grilled salmon or chicken salad (no heavy dressing)</li>";
        oss << "<li>Quinoa or lentil soup</li>";
    } else {
        oss << "<li>Mixed vegetable bowl with lean protein</li>";
        oss << "<li>Whole grain wrap or steamed rice</li>";
    }
    oss << "</ul>";

    oss << "<b>🌙 Dinner:</b><ul>";
    oss << "<li>Vegetable stir-fry with tofu or chicken</li>";
    oss << "<li>Light soup or side salad</li>";
    if (!conditions.empty() && conditions.find("diabet") != std::string::npos)
        oss << "<li>⚠️ Limit refined carbs — choose whole grains due to your condition.</li>";
    oss << "</ul>";

    oss << "<b>💧 Hydration:</b> Aim for at least <b>8 glasses</b> of water daily.<br><br>";
    oss << "<i>Consistency beats perfection. Small daily choices make a big difference! 🌱</i>";
    
    if (userToDoManager) {
        int id = userToDoManager->generateNewTaskID();
        userToDoManager->addTask(new DietTask(id, "Follow today's diet plan", "Dietary suggestion", "All meals"));
        oss << "<br><i>✅ Added 'Follow today's diet plan' to your tasks.</i>";
    }
    return oss.str();
}

std::string ChatBot::handleRecommendExercise() {
    if (!currentUserProfile) return "I need an active profile to recommend an exercise routine.";
    int mins = currentUserProfile->getPreferredExerciseDurationMins();
    if (mins <= 0) mins = 30;
    std::string goal = currentUserProfile->getHealthGoal();
    std::string name = currentUserProfile->getName();

    std::ostringstream oss;
    oss << "🏋️ <b>Exercise Plan for " << name << "</b><br>";
    oss << "<i>Goal: " << goal << " | Duration: " << mins << " min</i><br><br>";

    if (goal.find("muscle") != std::string::npos || goal.find("build") != std::string::npos) {
        oss << "<b>💪 Strength Training:</b><ul>";
        oss << "<li>5 min warm-up (jump rope or light jog)</li>";
        oss << "<li>Push-ups: 3 sets x 15 reps</li>";
        oss << "<li>Bodyweight squats: 3 sets x 20 reps</li>";
        oss << "<li>Dumbbell rows or resistance band pulls: 3 sets</li>";
        oss << "<li>Plank holds: 3 x 45 seconds</li>";
        oss << "<li>5 min cool-down stretch</li></ul>";
    } else if (goal.find("weight") != std::string::npos || goal.find("lose") != std::string::npos) {
        oss << "<b>🔥 Cardio &amp; Fat Burn:</b><ul>";
        oss << "<li>" << mins << " min brisk walk or light jog</li>";
        oss << "<li>10 min HIIT intervals (30s on / 30s rest)</li>";
        oss << "<li>Jumping jacks: 3 sets x 30 reps</li>";
        oss << "<li>Core crunches: 3 sets x 20 reps</li></ul>";
    } else {
        oss << "<b>🌿 General Wellness:</b><ul>";
        oss << "<li>" << mins << " min brisk walk outdoors</li>";
        oss << "<li>Light yoga or stretching (15 min)</li>";
        oss << "<li>Deep breathing exercises for stress relief</li></ul>";
    }

    oss << "<b>📌 Tip:</b> Aim for at least 5 days a week. Rest days are equally important! 😴<br><br>";
    oss << "<i>Listen to your body — stop if you feel pain or dizziness. Always consult a doctor before starting a new routine.</i>";

    if (userToDoManager) {
        int id = userToDoManager->generateNewTaskID();
        std::string taskType = (goal.find("muscle") != std::string::npos) ? "Strength" : "Cardio";
        userToDoManager->addTask(new ExerciseTask(id, "Complete today's workout", "Daily exercise", taskType, mins));
        oss << "<br><i>✅ Added 'Complete today's workout' to your tasks.</i>";
    }
    return oss.str();
}

std::string ChatBot::handleAddCustomTask(const std::string& originalTitle) {
    if (!userToDoManager) return "Task manager is not available right now.";
    int id = userToDoManager->generateNewTaskID();
    userToDoManager->addTask(new GenericTask(id, originalTitle, "User added"));
    std::vector<std::string> confirms = {
        "✅ Got it! <b>'" + originalTitle + "'</b> has been added to your task list.",
        "📌 <b>'" + originalTitle + "'</b> is now in your list. Stay on top of it! 💪",
        "✨ Task noted! <b>'" + originalTitle + "'</b> added. Good planning ahead!"
    };
    return getRandomResponse(confirms);
}

std::string ChatBot::handleViewTasks() {
    if (!userToDoManager) return "Task manager is not available right now.";
    auto tasks = userToDoManager->getAllTasks();
    if (tasks.empty()) {
        return "📋 <b>Your task list is empty.</b><br><br>You can ask me to <i>'recommend diet'</i> or <i>'recommend exercise'</i> to get tasks added, or type <i>'addtask Your Task Name'</i> to add your own!";
    }
    std::ostringstream ss;
    int done = 0, total = (int)tasks.size();
    for (auto t : tasks) if (t->getStatus()) done++;
    ss << "📋 <b>Your Health Tasks</b> (<span style='color:#2563EB;'>" << done << "/" << total << " completed</span>)<br><br>";
    for (auto t : tasks) {
        if (t->getStatus())
            ss << "<span style='color:#16A34A;'>✅</span> <s>" << t->getTitle() << "</s><br>";
        else
            ss << "<span style='color:#94A3B8;'>○</span> " << t->getTitle() << "<br>";
    }
    if (done == total)
        ss << "<br>🎉 <b>Excellent!</b> You've completed all your tasks today!";
    else
        ss << "<br><i>" << (total - done) << " task(s) remaining. You're doing great — keep going! 💪</i>";
    return ss.str();
}

std::string ChatBot::handleCompleteTask(const std::vector<std::string>& tokens) {
    if (!userToDoManager || tokens.size() < 2) return "Please specify task ID. e.g. 'complete 1'";
    try {
        int id = std::stoi(tokens[1]);
        userToDoManager->completeTask(id);
        std::vector<std::string> completes = {
            "✅ Excellent work! Task " + tokens[1] + " is done. You're on a roll!",
            "🎉 Task " + tokens[1] + " marked complete! Keep up the great momentum.",
            "✔️ Done! Task " + tokens[1] + " completed. Small wins lead to big results."
        };
        return getRandomResponse(completes);
    } catch (...) {
        return "Hmm, that doesn't look like a valid task ID. Please try again.";
    }
}

std::string ChatBot::handleHelp() {
    return "📊 <b>Here's what I can do for you:</b><br><br>"
           "<b>Health Topics:</b><br>"
           "&bull; Type a symptom — e.g. <i>'I have a headache'</i> or <i>'my stomach hurts'</i><br>"
           "&bull; Say <i>'recommend diet'</i> for a personalised meal plan<br>"
           "&bull; Say <i>'recommend exercise'</i> for a workout routine<br><br>"
           "<b>Info & Tracking:</b><br>"
           "&bull; <i>'Health Score'</i> — see your current wellness score<br>"
           "&bull; <i>'tasks'</i> — view your to-do list<br>"
           "&bull; <i>'profile'</i> — view your health profile<br><br>"
           "<b>Quick tip:</b> Use the suggestion chips below for fast actions! 👇";
}

std::string ChatBot::handleSummary() {
    return handleViewTasks();
}

std::string ChatBot::handleHealthScore() {
    HealthScore hs;
    hs.calculate(currentUserProfile, userToDoManager);
    std::vector<std::string> intros = {
        "📊 <b>Here's your Health Score snapshot:</b><br><br>",
        "🌟 <b>Your Wellness Report:</b><br><br>",
        "🩺 <b>Let's see how you're doing overall:</b><br><br>"
    };
    return getRandomResponse(intros) + hs.getScoreSummary();
}

std::string ChatBot::handleUnknownCommand(const std::string& originalCommandWord) {
    (void)originalCommandWord;
    std::vector<std::string> fallbacks = {
        "I'm still learning! 🎓 Could you try phrasing that as a symptom (like 'I have a headache')?",
        "Hmm, I didn't quite catch that. 🤔 You can ask for your 'Health Score' or describe a symptom.",
        "I'm not sure I understand. Try clicking one of the suggestion chips above! 👇",
        "Let's try that again. You can type 'recommend diet' or tell me how you're feeling today. 🌿"
    };
    return getRandomResponse(fallbacks);
}

const DietPlan& ChatBot::getLastGeneratedDietPlan() const { return lastGeneratedDietPlan; }
void ChatBot::clearLastDietPlan() { lastGeneratedDietPlan.clear(); }

// UI Getters
ConversationState ChatBot::getCurrentState() const { return currentConvState; }
std::vector<std::string> ChatBot::getGatheredSymptoms() const { return gatheredSymptoms; }
int ChatBot::getReportedSeverity() const { return reportedSeverity; }
std::vector<std::string> ChatBot::getSuggestionChips() const {
    if (currentConvState == ConversationState::IDLE) {
        return {"I have a headache", "My stomach hurts", "Recommend Diet", "Health Score"};
    } else if (currentConvState == ConversationState::SEVERITY_ASSESSMENT) {
        return {"Mild (2)", "Moderate (5)", "Severe (8)", "Unbearable (10)"};
    }
    return {};
}
