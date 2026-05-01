// File: mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>

// Forward declarations
class QStackedWidget;
class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QTextBrowser;
class QLineEdit;
class QSplitter;
class QScrollArea;
class QTabWidget;
class QSlider;

// Backend models/managers
class ProfileManager;
class ToDoManager;
class HistoryManager;
class ChatBot;
class UserProfile;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showProfileSelectScreen();
    void showCreateProfileDialog();
    void attemptLoadProfile();
    void deleteSelectedProfile();
    void showWelcomeView();
    void handleNewProfileCreatedAndSaved(const std::string& profileName);
    void devGoBackToProfileSelect();

    // Chat interactions
    void onSendChatMessage();
    void onSuggestionChipClicked();
    void onSeveritySliderChanged(int value);

    // Tasks
    void onTaskItemChanged(QListWidgetItem *item);

private:
    // UI Structure Elements
    QStackedWidget *mainStackedWidget;
    QWidget *welcomeScreenWidget;
    QWidget *profileSelectScreenWidget;
    QWidget *mainApplicationWidget;
    QListWidget *profileListWidget_onSelectScreen;

    // --- MainApplicationWidget UI Members ---
    // Header
    QLabel *headerScoreLabel;

    // Left Panel (Chat)
    QTextBrowser *chatHistoryBrowser;
    QLineEdit *chatInputLineEdit;
    QPushButton *sendChatMessageButton;
    QWidget *suggestionChipsContainer;
    QSlider *severitySlider;
    QLabel *severityValueLabel;

    // Right Panel (Info)
    QLabel *profileNameLabel;
    QLabel *profileAgeLabel;
    QLabel *profileGenderLabel;
    QLabel *profileWeightLabel;
    QLabel *profileHeightLabel;
    QLabel *profileConditionsLabel;
    QLabel *profileGoalLabel;

    QListWidget *toDoListWidget;

    // Health Score Dashboard
    QLabel *dashboardScoreLabel;
    QLabel *dashboardCategoryLabel;
    QLabel *dashboardDetailsLabel;

    // Footer
    QLabel *statusMessageLabel;
    // --- End of MainApplicationWidget UI Members ---

    // Backend Logic Instances
    ProfileManager *profileManager;
    ToDoManager *taskManager;
    HistoryManager *historyManager;
    ChatBot *chatBot;
    UserProfile *activeUserProfile;

    // Helper methods for UI setup
    void setupCoreUi();
    void setupWelcomeScreen();
    void setupProfileSelectScreen();
    void setupMainApplicationScreen();

    // Helper methods for logic
    void loadAndShowProfile(const std::string& profileName);
    void startUserSession();
    void refreshProfileListOnSelectScreen();

    void populateProfileDisplay();
    void populateTaskListDisplay();
    void updateHealthScoreDashboard();
    
    // Chat rendering
    void appendUserMessage(const QString& sender, const QString& message);
    void appendBotMessage(const QString& message, bool isEmergency = false);
    void refreshSuggestionChips();

    static std::string generateUserSpecificTaskFilename(const std::string& profileName);
};

#endif // MAINWINDOW_H
