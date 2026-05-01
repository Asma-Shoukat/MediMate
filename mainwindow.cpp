// File: mainwindow.cpp
#include "mainwindow.h"
#include "profiledetailsdialog.h"

// Qt Includes
#include <QApplication>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTextBrowser>
#include <QLineEdit>
#include <QSplitter>
#include <QScrollArea>
#include <QTabWidget>
#include <QMessageBox>
#include <QInputDialog>
#include <QFont>
#include <QSlider>
#include <QTimer>
#include <QScrollBar>
#include <QRegularExpression>

// Backend Includes
#include "core_logic/managers/profilemanager.h"
#include "core_logic/managers/todomanager.h"
#include "core_logic/managers/historymanager.h"
#include "core_logic/chatbot/chatbot.h"
#include "core_logic/models/userprofile.h"
#include "core_logic/models/healthtask.h"
#include "core_logic/models/healthscore.h"
#include "core_logic/models/exercisetask.h"
#include "core_logic/models/diettask.h"

std::string MainWindow::generateUserSpecificTaskFilename(const std::string& profileName) {
    std::string safeProfileName = profileName;
    std::replace(safeProfileName.begin(), safeProfileName.end(), ' ', '_');
    safeProfileName.erase(std::remove_if(safeProfileName.begin(), safeProfileName.end(),
                                         [](char c) { return !std::isalnum(c) && c != '_'; }),
                          safeProfileName.end());
    if (safeProfileName.empty()) safeProfileName = "default_user_tasks";
    return "tasks/tasks_" + safeProfileName + ".txt";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), mainStackedWidget(nullptr), welcomeScreenWidget(nullptr),
      profileSelectScreenWidget(nullptr), mainApplicationWidget(nullptr),
      profileListWidget_onSelectScreen(nullptr), headerScoreLabel(nullptr),
      chatHistoryBrowser(nullptr), chatInputLineEdit(nullptr), sendChatMessageButton(nullptr),
      suggestionChipsContainer(nullptr), severitySlider(nullptr), severityValueLabel(nullptr),
      profileNameLabel(nullptr), profileAgeLabel(nullptr), profileGenderLabel(nullptr),
      profileWeightLabel(nullptr), profileHeightLabel(nullptr), profileConditionsLabel(nullptr),
      profileGoalLabel(nullptr), toDoListWidget(nullptr), dashboardScoreLabel(nullptr),
      dashboardCategoryLabel(nullptr), dashboardDetailsLabel(nullptr), statusMessageLabel(nullptr),
      profileManager(nullptr), taskManager(nullptr), historyManager(nullptr),
      chatBot(nullptr), activeUserProfile(nullptr)
{
    profileManager = new ProfileManager("profiles/");
    taskManager = new ToDoManager();
    historyManager = new HistoryManager("history/");

    setupCoreUi();

    if (welcomeScreenWidget && mainStackedWidget) {
        mainStackedWidget->setCurrentWidget(welcomeScreenWidget);
    }
    setMinimumSize(950, 750);
    setWindowTitle("MediMate - AI Health Assistant");
}

MainWindow::~MainWindow() {
    delete chatBot;
    delete taskManager;
    delete profileManager;
    delete historyManager;
}

void MainWindow::setupCoreUi() {
    mainStackedWidget = new QStackedWidget(this);
    setupWelcomeScreen();
    setupProfileSelectScreen();
    setupMainApplicationScreen();
    setCentralWidget(mainStackedWidget);
}

void MainWindow::setupWelcomeScreen() {
    welcomeScreenWidget = new QWidget(mainStackedWidget);
    welcomeScreenWidget->setObjectName("welcomeScreen");
    
    // Two-column horizontal layout
    QHBoxLayout *rootLayout = new QHBoxLayout(welcomeScreenWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ── Left Brand Panel ──────────────────────────────────────────
    QWidget *brandPanel = new QWidget(welcomeScreenWidget);
    brandPanel->setObjectName("welcomeBrandPanel");
    brandPanel->setFixedWidth(440);
    QVBoxLayout *brandLayout = new QVBoxLayout(brandPanel);
    brandLayout->setContentsMargins(50, 60, 50, 60);
    brandLayout->setSpacing(0);

    // Badge
    QLabel *badgeLabel = new QLabel("AI HEALTH ASSISTANT", brandPanel);
    badgeLabel->setObjectName("welcomeBadgeLabel");
    badgeLabel->setAlignment(Qt::AlignLeft);

    // Logo emoji in a circle widget
    QLabel *logoLabel = new QLabel("🩺", brandPanel);
    logoLabel->setObjectName("welcomeLogo");
    QFont lf = logoLabel->font(); lf.setPointSize(52); logoLabel->setFont(lf);
    logoLabel->setAlignment(Qt::AlignLeft);

    // Title
    QLabel *titleLabel = new QLabel("MediMate", brandPanel);
    titleLabel->setObjectName("welcomeTitleLabel");
    titleLabel->setAlignment(Qt::AlignLeft);

    // Subtitle
    QLabel *stLabel = new QLabel("Your intelligent companion\nfor better health & wellness.", brandPanel);
    stLabel->setObjectName("welcomeSubtitleLabel");
    stLabel->setAlignment(Qt::AlignLeft);
    stLabel->setWordWrap(true);

    // Feature list
    QLabel *feat1 = new QLabel("✦  Symptom Analysis & Insights", brandPanel);
    feat1->setObjectName("welcomeFeatureLabel");
    QLabel *feat2 = new QLabel("✦  Personalised Diet & Exercise", brandPanel);
    feat2->setObjectName("welcomeFeatureLabel");
    QLabel *feat3 = new QLabel("✦  Real-time Health Scoring", brandPanel);
    feat3->setObjectName("welcomeFeatureLabel");

    brandLayout->addStretch(1);
    brandLayout->addWidget(badgeLabel);
    brandLayout->addSpacing(18);
    brandLayout->addWidget(logoLabel);
    brandLayout->addSpacing(10);
    brandLayout->addWidget(titleLabel);
    brandLayout->addSpacing(12);
    brandLayout->addWidget(stLabel);
    brandLayout->addSpacing(32);
    brandLayout->addWidget(feat1);
    brandLayout->addSpacing(8);
    brandLayout->addWidget(feat2);
    brandLayout->addSpacing(8);
    brandLayout->addWidget(feat3);
    brandLayout->addStretch(2);

    // Footer on brand panel
    QLabel *versionLabel = new QLabel("MediMate v2.0  •  Powered by AI", brandPanel);
    versionLabel->setObjectName("welcomeVersionLabel");
    versionLabel->setAlignment(Qt::AlignLeft);
    brandLayout->addWidget(versionLabel);

    // ── Right Action Panel ─────────────────────────────────────────
    QWidget *actionPanel = new QWidget(welcomeScreenWidget);
    actionPanel->setObjectName("welcomeActionPanel");
    QVBoxLayout *actionLayout = new QVBoxLayout(actionPanel);
    actionLayout->setContentsMargins(60, 0, 60, 0);
    actionLayout->setSpacing(0);

    QLabel *actionTitle = new QLabel("Get Started", actionPanel);
    actionTitle->setObjectName("welcomeActionTitle");
    actionTitle->setAlignment(Qt::AlignCenter);

    QLabel *actionSubtitle = new QLabel("Load your profile or create a new one\nto begin your wellness journey.", actionPanel);
    actionSubtitle->setObjectName("welcomeActionSubtitle");
    actionSubtitle->setAlignment(Qt::AlignCenter);
    actionSubtitle->setWordWrap(true);

    QPushButton *gsBtn = new QPushButton("  Load Profile", actionPanel);
    gsBtn->setObjectName("welcomePrimaryBtn");
    gsBtn->setMinimumHeight(52);
    gsBtn->setMinimumWidth(280);

    QLabel *orLabel = new QLabel("or", actionPanel);
    orLabel->setObjectName("welcomeOrLabel");
    orLabel->setAlignment(Qt::AlignCenter);

    QPushButton *cnpBtn = new QPushButton("  Create New Profile", actionPanel);
    cnpBtn->setObjectName("welcomeSecondaryBtn");
    cnpBtn->setMinimumHeight(52);
    cnpBtn->setMinimumWidth(280);

    QPushButton *exBtn = new QPushButton("Exit", actionPanel);
    exBtn->setObjectName("welcomeExitBtn");
    exBtn->setMinimumHeight(38);
    exBtn->setMinimumWidth(100);

    actionLayout->addStretch(2);
    actionLayout->addWidget(actionTitle, 0, Qt::AlignCenter);
    actionLayout->addSpacing(10);
    actionLayout->addWidget(actionSubtitle, 0, Qt::AlignCenter);
    actionLayout->addSpacing(50);
    actionLayout->addWidget(gsBtn, 0, Qt::AlignCenter);
    actionLayout->addSpacing(14);
    actionLayout->addWidget(orLabel, 0, Qt::AlignCenter);
    actionLayout->addSpacing(14);
    actionLayout->addWidget(cnpBtn, 0, Qt::AlignCenter);
    actionLayout->addStretch(2);
    actionLayout->addWidget(exBtn, 0, Qt::AlignCenter);
    actionLayout->addSpacing(20);

    rootLayout->addWidget(brandPanel);
    rootLayout->addWidget(actionPanel, 1);

    mainStackedWidget->addWidget(welcomeScreenWidget);
    connect(gsBtn, &QPushButton::clicked, this, &MainWindow::showProfileSelectScreen);
    connect(cnpBtn, &QPushButton::clicked, this, &MainWindow::showCreateProfileDialog);
    connect(exBtn, &QPushButton::clicked, qApp, &QApplication::quit);
}

void MainWindow::setupProfileSelectScreen() {
    profileSelectScreenWidget = new QWidget(mainStackedWidget);
    profileSelectScreenWidget->setObjectName("profileScreen");

    // Two-column layout — same brand panel + action panel pattern
    QHBoxLayout *rootLayout = new QHBoxLayout(profileSelectScreenWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ── Left Brand Panel (reuse same look) ───────────────────────
    QWidget *brandPanel2 = new QWidget(profileSelectScreenWidget);
    brandPanel2->setObjectName("welcomeBrandPanel");
    brandPanel2->setFixedWidth(440);
    QVBoxLayout *bl2 = new QVBoxLayout(brandPanel2);
    bl2->setContentsMargins(50, 60, 50, 60);
    bl2->setSpacing(0);

    QLabel *b2badge = new QLabel("AI HEALTH ASSISTANT", brandPanel2);
    b2badge->setObjectName("welcomeBadgeLabel");
    QLabel *b2logo = new QLabel("🩺", brandPanel2);
    b2logo->setObjectName("welcomeLogo");
    QFont lf2 = b2logo->font(); lf2.setPointSize(52); b2logo->setFont(lf2);
    QLabel *b2title = new QLabel("MediMate", brandPanel2);
    b2title->setObjectName("welcomeTitleLabel");
    QLabel *b2sub = new QLabel("Who are you today?\nChoose or create your profile to begin.", brandPanel2);
    b2sub->setObjectName("welcomeSubtitleLabel");
    b2sub->setWordWrap(true);

    bl2->addStretch(1);
    bl2->addWidget(b2badge);
    bl2->addSpacing(18);
    bl2->addWidget(b2logo);
    bl2->addSpacing(10);
    bl2->addWidget(b2title);
    bl2->addSpacing(12);
    bl2->addWidget(b2sub);
    bl2->addStretch(2);
    QLabel *b2ver = new QLabel("MediMate v2.0  •  Powered by AI", brandPanel2);
    b2ver->setObjectName("welcomeVersionLabel");
    bl2->addWidget(b2ver);
    bl2->addSpacing(20);

    // ── Right Profile Action Panel ────────────────────────────────
    QWidget *actionPanel2 = new QWidget(profileSelectScreenWidget);
    actionPanel2->setObjectName("welcomeActionPanel");
    QVBoxLayout *al2 = new QVBoxLayout(actionPanel2);
    al2->setContentsMargins(50, 40, 50, 30);
    al2->setSpacing(0);

    QLabel *a2Title = new QLabel("Select Profile", actionPanel2);
    a2Title->setObjectName("welcomeActionTitle");
    a2Title->setAlignment(Qt::AlignCenter);
    al2->addWidget(a2Title);
    al2->addSpacing(24);

    profileListWidget_onSelectScreen = new QListWidget(actionPanel2);
    profileListWidget_onSelectScreen->setObjectName("profileListWidget");
    refreshProfileListOnSelectScreen();
    al2->addWidget(profileListWidget_onSelectScreen, 1);
    al2->addSpacing(20);

    QPushButton *ldBtn = new QPushButton("  Load Selected Profile", actionPanel2);
    ldBtn->setObjectName("welcomePrimaryBtn");
    ldBtn->setMinimumHeight(48);
    QPushButton *crBtn = new QPushButton("  Create New Profile", actionPanel2);
    crBtn->setObjectName("welcomeSecondaryBtn");
    crBtn->setMinimumHeight(48);
    QPushButton *delBtn = new QPushButton("🗑  Delete Profile", actionPanel2);
    delBtn->setObjectName("welcomeDeleteBtn");
    delBtn->setMinimumHeight(40);
    QPushButton *bkwBtn = new QPushButton("← Back", actionPanel2);
    bkwBtn->setObjectName("welcomeExitBtn");
    bkwBtn->setMinimumHeight(38);

    QHBoxLayout *btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);
    btnRow->addWidget(crBtn);
    btnRow->addWidget(ldBtn);
    al2->addLayout(btnRow);
    al2->addSpacing(10);
    al2->addWidget(delBtn, 0, Qt::AlignCenter);
    al2->addSpacing(8);
    al2->addWidget(bkwBtn, 0, Qt::AlignCenter);
    al2->addSpacing(10);

    rootLayout->addWidget(brandPanel2);
    rootLayout->addWidget(actionPanel2, 1);

    mainStackedWidget->addWidget(profileSelectScreenWidget);
    connect(crBtn, &QPushButton::clicked, this, &MainWindow::showCreateProfileDialog);
    connect(ldBtn, &QPushButton::clicked, this, &MainWindow::attemptLoadProfile);
    connect(delBtn, &QPushButton::clicked, this, &MainWindow::deleteSelectedProfile);
    connect(bkwBtn, &QPushButton::clicked, this, &MainWindow::showWelcomeView);
}

void MainWindow::setupMainApplicationScreen() {
    mainApplicationWidget = new QWidget(mainStackedWidget);
    QVBoxLayout *mainAppOverallLayout = new QVBoxLayout(mainApplicationWidget);
    mainAppOverallLayout->setContentsMargins(10,10,10,10);
    mainAppOverallLayout->setSpacing(10);

    // --- Header ---
    QWidget *headerWidget = new QWidget(mainApplicationWidget);
    headerWidget->setObjectName("appHeader");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(15, 10, 15, 10);
    
    QLabel *appTitle = new QLabel("<b>MediMate</b> AI", headerWidget);
    appTitle->setObjectName("appHeaderTitle");
    headerScoreLabel = new QLabel("Score: --", headerWidget);
    headerScoreLabel->setObjectName("headerScoreLabel");
    
    QPushButton *emergencyBtn = new QPushButton("🚨 Emergency", headerWidget);
    emergencyBtn->setObjectName("emergencyHeaderBtn");
    
    headerLayout->addWidget(appTitle);
    headerLayout->addStretch();
    headerLayout->addWidget(headerScoreLabel);
    headerLayout->addWidget(emergencyBtn);
    
    mainAppOverallLayout->addWidget(headerWidget);

    // --- Splitter ---
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, mainApplicationWidget);
    mainSplitter->setObjectName("mainSplitter");

    // --- Left Panel: Chat Area ---
    QWidget *chatAreaWidget = new QWidget(mainSplitter);
    chatAreaWidget->setObjectName("chatAreaWidget");
    QVBoxLayout *chatAreaLayout = new QVBoxLayout(chatAreaWidget);
    chatAreaLayout->setContentsMargins(0,0,0,0);
    chatAreaLayout->setSpacing(10);
    
    chatHistoryBrowser = new QTextBrowser(chatAreaWidget);
    chatHistoryBrowser->setObjectName("chatHistoryBrowser");
    chatHistoryBrowser->setOpenExternalLinks(true);
    
    // Chips Container
    suggestionChipsContainer = new QWidget(chatAreaWidget);
    QHBoxLayout *chipsLayout = new QHBoxLayout(suggestionChipsContainer);
    chipsLayout->setContentsMargins(0,0,0,0);
    chipsLayout->setSpacing(8);
    chipsLayout->addStretch(); // Align chips to right or center based on styling
    
    // Severity Slider Container
    QWidget *sliderWidget = new QWidget(chatAreaWidget);
    sliderWidget->setObjectName("sliderWidget");
    QHBoxLayout *sliderLayout = new QHBoxLayout(sliderWidget);
    sliderLayout->setContentsMargins(0,0,0,0);
    QLabel *sliderIcon = new QLabel("Severity:", sliderWidget);
    severitySlider = new QSlider(Qt::Horizontal, sliderWidget);
    severitySlider->setRange(1, 10);
    severitySlider->setValue(5);
    severitySlider->setTickPosition(QSlider::TicksBelow);
    severityValueLabel = new QLabel("5", sliderWidget);
    sliderLayout->addWidget(sliderIcon);
    sliderLayout->addWidget(severitySlider);
    sliderLayout->addWidget(severityValueLabel);
    sliderWidget->setVisible(false); // Hidden by default
    
    // Input Row
    QHBoxLayout *chatInputRowLayout = new QHBoxLayout();
    chatInputLineEdit = new QLineEdit(chatAreaWidget);
    chatInputLineEdit->setObjectName("chatInputLineEdit");
    chatInputLineEdit->setPlaceholderText("Describe your symptoms or ask a question...");
    sendChatMessageButton = new QPushButton("Send", chatAreaWidget);
    sendChatMessageButton->setObjectName("sendChatMessageButton");
    chatInputRowLayout->addWidget(chatInputLineEdit, 1);
    chatInputRowLayout->addWidget(sendChatMessageButton);
    
    chatAreaLayout->addWidget(chatHistoryBrowser, 1);
    chatAreaLayout->addWidget(suggestionChipsContainer);
    chatAreaLayout->addWidget(sliderWidget);
    chatAreaLayout->addLayout(chatInputRowLayout);
    chatAreaWidget->setLayout(chatAreaLayout);
    mainSplitter->addWidget(chatAreaWidget);

    // --- Right Panel: Info Tabs ---
    QTabWidget *infoTabWidget = new QTabWidget(mainSplitter);
    infoTabWidget->setObjectName("infoTabWidget");

    // Profile Tab
    QWidget *profileTab = new QWidget(infoTabWidget);
    QVBoxLayout *profileTabLayout = new QVBoxLayout(profileTab);
    QScrollArea *profileScrollArea = new QScrollArea(profileTab);
    profileScrollArea->setWidgetResizable(true);
    QWidget *profileScrollWidgetContents = new QWidget();
    QGridLayout *profileDetailsLayout = new QGridLayout(profileScrollWidgetContents);
    profileDetailsLayout->addWidget(new QLabel("Name:", profileScrollWidgetContents), 0, 0); profileNameLabel = new QLabel("N/A", profileScrollWidgetContents); profileDetailsLayout->addWidget(profileNameLabel, 0, 1);
    profileDetailsLayout->addWidget(new QLabel("Age:", profileScrollWidgetContents), 1, 0); profileAgeLabel = new QLabel("N/A", profileScrollWidgetContents); profileDetailsLayout->addWidget(profileAgeLabel, 1, 1);
    profileDetailsLayout->addWidget(new QLabel("Gender:", profileScrollWidgetContents), 2, 0); profileGenderLabel = new QLabel("N/A", profileScrollWidgetContents); profileDetailsLayout->addWidget(profileGenderLabel, 2, 1);
    profileDetailsLayout->addWidget(new QLabel("Weight:", profileScrollWidgetContents), 3, 0); profileWeightLabel = new QLabel("N/A", profileScrollWidgetContents); profileDetailsLayout->addWidget(profileWeightLabel, 3, 1);
    profileDetailsLayout->addWidget(new QLabel("Height:", profileScrollWidgetContents), 4, 0); profileHeightLabel = new QLabel("N/A", profileScrollWidgetContents); profileDetailsLayout->addWidget(profileHeightLabel, 4, 1);
    profileDetailsLayout->addWidget(new QLabel("Conditions:", profileScrollWidgetContents), 5, 0, Qt::AlignTop); profileConditionsLabel = new QLabel("N/A", profileScrollWidgetContents); profileConditionsLabel->setWordWrap(true); profileDetailsLayout->addWidget(profileConditionsLabel, 5, 1);
    profileDetailsLayout->addWidget(new QLabel("Goal:", profileScrollWidgetContents), 6, 0, Qt::AlignTop); profileGoalLabel = new QLabel("N/A", profileScrollWidgetContents); profileGoalLabel->setWordWrap(true); profileDetailsLayout->addWidget(profileGoalLabel, 6, 1);
    profileDetailsLayout->setColumnStretch(1, 1);
    profileDetailsLayout->setRowStretch(8, 1);
    profileScrollWidgetContents->setLayout(profileDetailsLayout);
    profileScrollArea->setWidget(profileScrollWidgetContents);
    profileTabLayout->addWidget(profileScrollArea);
    profileTab->setLayout(profileTabLayout);
    infoTabWidget->addTab(profileTab, "Profile");

    // Tasks Tab
    QWidget *toDoTab = new QWidget(infoTabWidget);
    QVBoxLayout *toDoTabLayout = new QVBoxLayout(toDoTab);
    toDoListWidget = new QListWidget(toDoTab);
    toDoListWidget->setObjectName("todoListWidget");
    toDoTabLayout->addWidget(new QLabel("<b>Health Tasks</b>"));
    toDoTabLayout->addWidget(toDoListWidget, 1);
    toDoTab->setLayout(toDoTabLayout);
    infoTabWidget->addTab(toDoTab, "Tasks");
    
    // Dashboard Tab (Health Score)
    QWidget *dashboardTab = new QWidget(infoTabWidget);
    QVBoxLayout *dashLayout = new QVBoxLayout(dashboardTab);
    dashLayout->setSpacing(15);
    
    QWidget *scoreCard = new QWidget(dashboardTab);
    scoreCard->setObjectName("scoreCard");
    QVBoxLayout *cardLayout = new QVBoxLayout(scoreCard);
    
    dashboardScoreLabel = new QLabel("0", scoreCard);
    dashboardScoreLabel->setObjectName("dashboardScoreValue");
    dashboardScoreLabel->setAlignment(Qt::AlignCenter);
    
    dashboardCategoryLabel = new QLabel("Category", scoreCard);
    dashboardCategoryLabel->setObjectName("dashboardCategoryLabel");
    dashboardCategoryLabel->setAlignment(Qt::AlignCenter);
    
    cardLayout->addWidget(new QLabel("<b>Overall Health Score</b>"));
    cardLayout->addWidget(dashboardScoreLabel);
    cardLayout->addWidget(dashboardCategoryLabel);
    
    dashboardDetailsLabel = new QLabel("Details will appear here...", dashboardTab);
    dashboardDetailsLabel->setWordWrap(true);
    dashboardDetailsLabel->setAlignment(Qt::AlignTop);
    
    dashLayout->addWidget(scoreCard);
    dashLayout->addWidget(dashboardDetailsLabel, 1);
    dashboardTab->setLayout(dashLayout);
    infoTabWidget->addTab(dashboardTab, "Dashboard");

    mainSplitter->addWidget(infoTabWidget);
    mainSplitter->setSizes({600, 300}); // 2:1 ratio

    // --- Footer ---
    QHBoxLayout *bottomBarLayout = new QHBoxLayout();
    statusMessageLabel = new QLabel("Status: Ready.", mainApplicationWidget);
    statusMessageLabel->setObjectName("statusMessageLabel");
    statusMessageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QPushButton* devBackButton = new QPushButton("Switch User", mainApplicationWidget);
    devBackButton->setObjectName("devBackButton");
    bottomBarLayout->addWidget(statusMessageLabel); 
    bottomBarLayout->addWidget(devBackButton);
    
    mainAppOverallLayout->addWidget(mainSplitter, 1);
    mainAppOverallLayout->addLayout(bottomBarLayout);
    mainApplicationWidget->setLayout(mainAppOverallLayout);
    mainStackedWidget->addWidget(mainApplicationWidget);

    connect(sendChatMessageButton, &QPushButton::clicked, this, &MainWindow::onSendChatMessage);
    connect(chatInputLineEdit, &QLineEdit::returnPressed, this, &MainWindow::onSendChatMessage);
    connect(toDoListWidget, &QListWidget::itemChanged, this, &MainWindow::onTaskItemChanged);
    connect(devBackButton, &QPushButton::clicked, this, &MainWindow::devGoBackToProfileSelect);
    connect(severitySlider, &QSlider::valueChanged, this, &MainWindow::onSeveritySliderChanged);
    connect(emergencyBtn, &QPushButton::clicked, this, [this](){ 
        chatInputLineEdit->setText("emergency");
        onSendChatMessage();
    });
}

void MainWindow::startUserSession() {
    if (!activeUserProfile || !taskManager || !profileManager) return;
    this->setWindowTitle("MediMate - " + QString::fromStdString(activeUserProfile->getName()));
    
    // Load Tasks
    std::string userTaskFile = generateUserSpecificTaskFilename(activeUserProfile->getName());
    std::vector<int> taskIdsToRemove; 
    for(HealthTask* task : taskManager->getAllTasks()) { if(task) taskIdsToRemove.push_back(task->getTaskID());}
    for(int id : taskIdsToRemove) { taskManager->removeTask(id); }
    taskManager->loadTasksFromFile(userTaskFile);
    
    // Load History
    historyManager->loadSessionHistory(activeUserProfile->getName());
    
    delete chatBot; 
    chatBot = new ChatBot(activeUserProfile, taskManager, historyManager);
    
    populateProfileDisplay(); 
    populateTaskListDisplay();
    updateHealthScoreDashboard();
    
    if(chatHistoryBrowser) {
        chatHistoryBrowser->clear();
        // Load recent history into UI
        auto recent = historyManager->getRecentEntries(20);
        for (const auto& entry : recent) {
            if (entry.getSenderRole() == SenderRole::USER) {
                appendUserMessage(QString::fromStdString(entry.getSenderName()), QString::fromStdString(entry.getMessage()));
            } else {
                appendBotMessage(QString::fromStdString(entry.getMessage()), entry.getSenderRole() == SenderRole::EMERGENCY);
            }
        }
        
        if (recent.empty()) {
            appendBotMessage("Hello " + QString::fromStdString(activeUserProfile->getName()) + "! How can I assist you today? You can describe your symptoms or ask for a health score.");
        }
    }
    
    refreshSuggestionChips();
    statusMessageLabel->setText("Session started for " + QString::fromStdString(activeUserProfile->getName()));
    mainStackedWidget->setCurrentWidget(mainApplicationWidget);
}

void MainWindow::populateProfileDisplay() { 
    if (!activeUserProfile) return; 
    profileNameLabel->setText(QString::fromStdString(activeUserProfile->getName())); 
    profileAgeLabel->setText(QString::number(activeUserProfile->getAge())); 
    profileGenderLabel->setText(QString::fromStdString(activeUserProfile->getGender())); 
    profileWeightLabel->setText(QString::number(activeUserProfile->getWeightLb()) + " lb"); 
    profileHeightLabel->setText(QString::fromStdString(activeUserProfile->getHeightFtIn())); 
    profileConditionsLabel->setText(QString::fromStdString(activeUserProfile->getHealthConditions())); 
    profileGoalLabel->setText(QString::fromStdString(activeUserProfile->getHealthGoal())); 
}

void MainWindow::populateTaskListDisplay() { 
    if (!toDoListWidget || !taskManager) return; 
    toDoListWidget->blockSignals(true); 
    toDoListWidget->clear(); 
    auto tasks = taskManager->getAllTasks(); 
    if (tasks.empty()) { 
        QListWidgetItem *emptyItem = new QListWidgetItem("No tasks yet."); 
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsUserCheckable); 
        toDoListWidget->addItem(emptyItem); 
    } else { 
        for (HealthTask* task : tasks) { 
            if (task) { 
                QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(task->getTitle()), toDoListWidget); 
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable); 
                item->setCheckState(task->getStatus() ? Qt::Checked : Qt::Unchecked); 
                item->setData(Qt::UserRole, task->getTaskID()); 
            } 
        } 
    } 
    toDoListWidget->blockSignals(false);  
}

void MainWindow::updateHealthScoreDashboard() {
    HealthScore hs;
    hs.calculate(activeUserProfile, taskManager);
    
    dashboardScoreLabel->setText(QString::number(hs.getOverallScore()));
    dashboardScoreLabel->setStyleSheet("color: " + QString::fromStdString(hs.getScoreColor()) + "; font-size: 48px; font-weight: bold;");
    dashboardCategoryLabel->setText(QString::fromStdString(hs.getScoreCategory()));
    dashboardCategoryLabel->setStyleSheet("color: " + QString::fromStdString(hs.getScoreColor()) + "; font-size: 18px;");
    
    QString details = QString::fromStdString(hs.getScoreSummary());
    details.replace("\n", "<br>");
    dashboardDetailsLabel->setText(details);
    
    headerScoreLabel->setText("Score: " + QString::number(hs.getOverallScore()));
    headerScoreLabel->setStyleSheet("color: " + QString::fromStdString(hs.getScoreColor()) + "; font-weight: bold;");
}

void MainWindow::appendUserMessage(const QString& sender, const QString& message) { 
    Q_UNUSED(sender);
    QString html = "<table width='100%' border='0'><tr><td align='right'>"
                   "<table border='0' bgcolor='#2563EB' cellpadding='10' cellspacing='0' style='border-radius:12px;'>"
                   "<tr><td style='color: #FFFFFF; font-size: 11pt;'>" + message.toHtmlEscaped() + "</td></tr>"
                   "</table></td></tr></table>";
    chatHistoryBrowser->append(html); 
    QScrollBar *sb = chatHistoryBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::appendBotMessage(const QString& message, bool isEmergency) { 
    QString bg = isEmergency ? "#FEE2E2" : "#F1F5F9"; 
    QString color = isEmergency ? "#B91C1C" : "#0F172A";
    
    QString html = "<table width='100%' border='0'><tr><td align='left'>"
                   "<table border='0' bgcolor='" + bg + "' cellpadding='12' cellspacing='0'>"
                   "<tr><td style='color: " + color + "; font-size: 11pt;'>" + message + "</td></tr>"
                   "</table></td></tr></table>";
                   
    chatHistoryBrowser->append(html); 
    QScrollBar *sb = chatHistoryBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void MainWindow::onSendChatMessage() {
    if (!chatBot || !chatInputLineEdit) return;
    QString userInput = chatInputLineEdit->text().trimmed();
    if (userInput.isEmpty()) return;
    
    // If severity slider is visible, append its value
    if (severitySlider->parentWidget()->isVisible()) {
        userInput = QString::number(severitySlider->value());
        severitySlider->parentWidget()->setVisible(false); // Hide after use
    }
    
    appendUserMessage(QString::fromStdString(activeUserProfile->getName()), userInput);
    chatInputLineEdit->clear();
    
    std::string botResponseStr = chatBot->processInput(userInput.toStdString());
    
    // Add small delay for realistic "thinking"
    QTimer::singleShot(400, this, [this, botResponseStr]() {
        appendBotMessage(QString::fromStdString(botResponseStr), chatBot->getCurrentState() == ConversationState::EMERGENCY_DETECTED);
        
        populateTaskListDisplay();
        updateHealthScoreDashboard();
        refreshSuggestionChips();
        
        // Show severity slider if needed
        severitySlider->parentWidget()->setVisible(chatBot->getCurrentState() == ConversationState::SEVERITY_ASSESSMENT);
        if (chatBot->getCurrentState() == ConversationState::SEVERITY_ASSESSMENT) {
            chatInputLineEdit->setPlaceholderText("Or use the slider above...");
        } else {
            chatInputLineEdit->setPlaceholderText("Describe your symptoms or ask a question...");
        }
        
        QString statusText = QString::fromStdString(botResponseStr);
        statusText.replace(QRegularExpression("<[^>]*>"), ""); // Strip HTML
        if(statusText.length() > 60) statusText = statusText.left(57) + "...";
        statusMessageLabel->setText("Bot: " + statusText);
    });
}

void MainWindow::onSuggestionChipClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        chatInputLineEdit->setText(btn->text());
        onSendChatMessage();
    }
}

void MainWindow::onSeveritySliderChanged(int value) {
    severityValueLabel->setText(QString::number(value));
}

void MainWindow::refreshSuggestionChips() {
    if (!chatBot || !suggestionChipsContainer) return;
    
    QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(suggestionChipsContainer->layout());
    if (!layout) return;
    
    // Clear old chips
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) { delete child->widget(); }
        delete child;
    }
    layout->addStretch();
    
    auto chips = chatBot->getSuggestionChips();
    for (const auto& chipStr : chips) {
        QPushButton *chip = new QPushButton(QString::fromStdString(chipStr));
        chip->setObjectName("suggestionChip");
        connect(chip, &QPushButton::clicked, this, &MainWindow::onSuggestionChipClicked);
        layout->insertWidget(layout->count() - 1, chip);
    }
}

void MainWindow::onTaskItemChanged(QListWidgetItem *item) { 
    if (!item || !taskManager) return; 
    int taskID = item->data(Qt::UserRole).toInt(); 
    HealthTask* task = taskManager->getTask(taskID); 
    if (task) { 
        bool wasCompleted = task->getStatus(); 
        bool isNowChecked = (item->checkState() == Qt::Checked); 
        if (isNowChecked && !wasCompleted) { 
            taskManager->completeTask(taskID); 
            appendBotMessage("<i>Task '" + QString::fromStdString(task->getTitle()) + "' completed! " + QString::fromStdString(task->getCheerMessage()) + "</i>"); 
            updateHealthScoreDashboard();
        } else if (!isNowChecked && wasCompleted) { 
            item->setCheckState(Qt::Checked); 
        } 
    } 
}

void MainWindow::showWelcomeView() { if (mainStackedWidget) mainStackedWidget->setCurrentWidget(welcomeScreenWidget); }
void MainWindow::showProfileSelectScreen() { refreshProfileListOnSelectScreen(); if (mainStackedWidget) mainStackedWidget->setCurrentWidget(profileSelectScreenWidget); }
void MainWindow::showCreateProfileDialog() { 
    bool nameOk; 
    QString profileNameQStr = QInputDialog::getText(this, "Create Profile", "Enter unique profile name:", QLineEdit::Normal, "", &nameOk); 
    if (!nameOk || profileNameQStr.trimmed().isEmpty()) return; 
    std::string profileNameStd = profileNameQStr.trimmed().toStdString(); 
    
    ProfileDetailsDialog detailsDialog(profileNameQStr, this); 
    if (detailsDialog.exec() == QDialog::Accepted) { 
        profileManager->createNewActiveProfile(); 
        activeUserProfile = profileManager->getActiveProfile(); 
        activeUserProfile->setName(profileNameStd); 
        detailsDialog.applyToUserProfile(activeUserProfile); 
        if (profileManager->saveCurrentProfile()) { 
            handleNewProfileCreatedAndSaved(activeUserProfile->getName()); 
        } 
    } 
}

void MainWindow::handleNewProfileCreatedAndSaved(const std::string& profileName){ refreshProfileListOnSelectScreen(); loadAndShowProfile(profileName); }

void MainWindow::attemptLoadProfile() { 
    if (!profileListWidget_onSelectScreen) return; 
    QListWidgetItem *selectedItem = profileListWidget_onSelectScreen->currentItem(); 
    if (selectedItem) loadAndShowProfile(selectedItem->text().toStdString()); 
    else QMessageBox::information(this, "No Selection", "Please select a profile to load."); 
}

void MainWindow::deleteSelectedProfile() {
    if (!profileListWidget_onSelectScreen || !profileManager) return;
    QListWidgetItem *selectedItem = profileListWidget_onSelectScreen->currentItem();
    if (!selectedItem) {
        QMessageBox::information(this, "No Selection", "Please select a profile to delete.");
        return;
    }
    QString profileName = selectedItem->text();
    int ret = QMessageBox::warning(this, "Delete Profile",
        "Are you sure you want to permanently delete the profile <b>\"" + profileName + "\"</b>?<br>"
        "This action cannot be undone.",
        QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Yes) {
        if (profileManager->deleteProfile(profileName.toStdString())) {
            QMessageBox::information(this, "Deleted", "Profile \"" + profileName + "\" has been deleted.");
            refreshProfileListOnSelectScreen();
        } else {
            QMessageBox::critical(this, "Error", "Could not delete the profile. Please try again.");
        }
    }
}

void MainWindow::loadAndShowProfile(const std::string& profileName) { 
    if (profileManager->loadProfile(profileName)) { 
        activeUserProfile = profileManager->getActiveProfile(); 
        if (activeUserProfile) startUserSession(); 
    } else { 
        QMessageBox::critical(this, "Error", "Failed to load profile."); 
    } 
}

void MainWindow::devGoBackToProfileSelect() { 
    if (activeUserProfile && taskManager && profileManager && historyManager) { 
        taskManager->saveTasksToFile(generateUserSpecificTaskFilename(activeUserProfile->getName())); 
        profileManager->saveCurrentProfile();
        historyManager->saveSessionHistory(activeUserProfile->getName());
    } 
    delete chatBot; chatBot = nullptr; 
    activeUserProfile = nullptr; 
    if(profileManager) profileManager->clearActiveProfile(); 
    refreshProfileListOnSelectScreen(); 
    if (mainStackedWidget) mainStackedWidget->setCurrentWidget(profileSelectScreenWidget); 
    this->setWindowTitle("MediMate - AI Health Assistant");
}

void MainWindow::refreshProfileListOnSelectScreen() { 
    if (!profileListWidget_onSelectScreen || !profileManager) return;
    profileListWidget_onSelectScreen->clear(); 
    std::vector<std::string> profileNames = profileManager->listProfileNames(); 
    if (profileNames.empty()) profileListWidget_onSelectScreen->addItem("No profiles yet. Create one!"); 
    else { for (const std::string& name : profileNames) profileListWidget_onSelectScreen->addItem(QString::fromStdString(name)); } 
}
