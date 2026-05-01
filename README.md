<div align="center">

<br>

<img width="80" src="https://raw.githubusercontent.com/microsoft/fluentui-emoji/main/assets/Stethoscope/3D/stethoscope_3d.png" alt="MediMate Logo"/>

<h1>MediMate AI</h1>

<p><em>A professional, offline AI health assistant built with Qt 6 & C++17</em></p>

<br>

[![Qt](https://img.shields.io/badge/Qt-6.x-41CD52?style=for-the-badge&logo=qt&logoColor=white)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white)](#)
[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](#license)

<br>

</div>

---

## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Architecture](#-architecture)
- [Symptom Knowledge Base](#-symptom-knowledge-base)
- [Project Structure](#-project-structure)
- [Getting Started](#-getting-started)
- [How It Works](#-how-it-works)
- [Tech Stack](#-tech-stack)
- [Author](#-author)
- [Disclaimer](#-disclaimer)

---

## 🌟 Overview

**MediMate AI** is a fully offline, privacy-first desktop health assistant. No APIs, no cloud, no data leaving your machine.

It features a context-aware conversational AI engine built on a **finite state machine**, a structured **symptom knowledge base**, real-time **health scoring**, personalised **diet & exercise planning**, and a polished **two-panel Premium Light UI** — all written from scratch in C++17 with Qt 6.

> Built as a complete, production-quality Qt desktop application — not a toy project.

---

## ✨ Features

### 🤖 Intelligent Chat Engine
- **Multi-turn Conversations** — the bot maintains context across messages using a 6-state conversation machine
- **Symptom Analysis Pipeline** — tokenizes input, matches against a structured knowledge base, asks follow-up questions, and generates a formatted diagnosis card
- **Emergency Interrupt System** — scans every message for 18+ life-threatening keywords (heart attack, stroke, choking, seizure, etc.) and halts normal flow with immediate emergency instructions
- **Randomised Personality** — greeting, farewell, and fallback responses are drawn from pools to feel natural

### 🩺 Health Tools
- **Diet Planner** — generates a complete 3-meal plan (Breakfast / Lunch / Dinner) dynamically adapted to the user's health goal (muscle building, weight loss, or general wellness), with automatic warnings for conditions like diabetes
- **Exercise Coach** — produces structured workout routines (Strength, Cardio HIIT, or Wellness walks) based on the user's goal and preferred duration
- **Health Score Dashboard** — composite 0–100 wellness score across 4 dimensions: Profile Completeness, Task Completion, Activity Level, and General Wellness
- **Task Manager** — auto-adds health tasks from recommendations; UI shows a live ✅/○ progress checklist

### 👤 Profile System
- Create, load, and **delete** user profiles with confirmation dialogs
- Stores name, age, gender, weight, height, health conditions, goals, and exercise preferences
- Data is persisted locally as plain text files via `std::filesystem` (C++17)

### 💬 Session History
- Every conversation is saved per user via `HistoryManager`
- Messages are tagged with `SenderRole` (USER / BOT / EMERGENCY) for structured replay

### 🎨 Premium UI Design
- Two-panel **welcome screen**: Royal Blue gradient brand panel + clean white action panel
- Chat bubbles rendered in `QTextBrowser` using nested HTML `<table>` layout (workaround for Qt's CSS limitations)
- Suggestion chips for one-click quick actions
- Interactive severity slider for symptom assessment
- Health score displayed in a live dashboard tab
- Sleek, minimal scrollbars and modern tab styling

---

## 🖥️ Screenshots

<div align="center">
  <img src="main_screen.png" alt="Main Action Screen" width="45%">
  &nbsp; &nbsp; &nbsp;
  <img src="chat_screen.png" alt="Chatting Interface" width="45%">
</div>

> **Tip:** Replace `main_screen.png` and `chat_screen.png` in the repository with your actual screenshots to display them here!

---

## 🏗️ Architecture

### Conversation State Machine

```
┌──────────────────────────────────────────────────────────┐
│                        IDLE                              │
│         (waiting for user input / command routing)       │
└──────────────────────┬───────────────────────────────────┘
                       │ symptom detected
                       ▼
┌─────────────────────────────────────────┐
│           SEVERITY_ASSESSMENT           │
│   "On a scale of 1–10, how severe?"     │
└─────────────────┬───────────────────────┘
                  │ severity captured
                  ▼
┌─────────────────────────────────────────┐
│           SYMPTOM_GATHERING             │
│   Follow-up questions (1–2 per symptom) │
└─────────────────┬───────────────────────┘
                  │ all questions answered
                  ▼
┌─────────────────────────────────────────┐
│          PROVIDING_DIAGNOSIS            │
│   Formatted card: causes, actions,      │
│   severity badge, severity-aware advice │
└─────────────────────────────────────────┘

         ┌─────────────────────────────┐
         │     EMERGENCY_DETECTED      │  ← interrupts at ANY state
         │  Immediate alert + call 911 │
         └─────────────────────────────┘
```

### Component Architecture

```
MainWindow (Qt MVC Controller)
├── ChatBot (AI Engine)
│   ├── initializeSymptomDatabase()  — knowledge base
│   ├── initializeEmergencyKeywords() — 18 critical triggers
│   ├── processInput()               — state-machine router
│   ├── handleRecommendDiet()        — goal-aware meal plan
│   ├── handleRecommendExercise()    — goal-aware workout plan
│   └── getSuggestionChips()         — dynamic UI chip hints
├── ProfileManager                  — CRUD on user profiles
├── ToDoManager                     — health task lifecycle
├── HistoryManager                  — per-user session logs
└── HealthScore                     — composite scoring model
    ├── profileCompletenessScore  (0–25)
    ├── taskCompletionScore       (0–25)
    ├── activityScore             (0–25)
    └── wellnessScore             (0–25)
```

---

## 🧠 Symptom Knowledge Base

Each `SymptomEntry` stores:

| Field | Description |
|---|---|
| `symptomName` | Canonical symptom name |
| `possibleCauses` | Array of potential medical causes |
| `defaultSeverity` | `"low"` / `"medium"` / `"high"` |
| `followUpQuestions` | Contextual follow-up questions |
| `recommendedActions` | Actionable health advice |
| `redFlags` | Dangerous symptoms to watch for |
| `isEmergency` | Triggers immediate emergency response |

**Currently mapped symptoms:**

| Symptom | Severity | Emergency? |
|---|---|---|
| Headache / Migraine | Low | No |
| Fever | Medium | No |
| Cough | Low | No |
| Chest Pain | **High** | ✅ Yes |
| Nausea / Vomiting | Medium | No |
| Stomach ache | Medium | No |

**Emergency keywords detected (18+):** `chest pain`, `heart attack`, `can't breathe`, `stroke`, `face drooping`, `seizure`, `choking`, `suicide`, `passed out`, and more.

---

## 🗂️ Project Structure

```
MediMate/
│
├── 📄 MediMate.pro              # Qt project file (C++17, Qt Widgets)
├── 📄 main.cpp                  # App entry, stylesheet loader
├── 📄 mainwindow.cpp/.h         # Main UI controller (2500+ lines)
├── 📄 profiledetailsdialog.cpp  # Profile creation dialog
├── 📄 stylesheet.qss            # Premium Light Theme (400+ lines)
├── 📄 resources.qrc             # Qt resource bundle
│
├── 📁 core_logic/
│   │
│   ├── 📁 chatbot/
│   │   ├── chatbot.h            # State machine, structs, interface
│   │   └── chatbot.cpp          # Full AI engine (676 lines)
│   │
│   ├── 📁 managers/
│   │   ├── profilemanager.h/.cpp    # Profile CRUD + filesystem
│   │   ├── todomanager.h/.cpp       # Task lifecycle management
│   │   └── historymanager.h/.cpp    # Session history persistence
│   │
│   ├── 📁 models/
│   │   ├── userprofile.h/.cpp       # Health profile data model
│   │   ├── healthscore.h/.cpp       # Composite scoring engine
│   │   ├── healthtask.h/.cpp        # Base task class
│   │   ├── diettask.h/.cpp          # Diet-specific task subclass
│   │   ├── exercisetask.h/.cpp      # Exercise-specific task subclass
│   │   ├── generictask.h/.cpp       # Generic user task subclass
│   │   └── conversationentry.h/.cpp # Chat message model + serialization
│   │
│   └── 📁 persistence/
│       └── file_manager.h/.cpp      # File I/O for profiles & tasks
│
└── 📁 resources/
    └── icons/                       # Checkbox icons
```

---

## 🚀 Getting Started

### Prerequisites

| Requirement | Version |
|---|---|
| Qt Creator | 10.x or later |
| Qt Framework | **6.x** (Qt Widgets module) |
| Compiler | MSVC 2019+ / MinGW 11+ |
| C++ Standard | **C++17** (required for `std::filesystem`) |
| OS | Windows 10/11 (primary target) |

### Build & Run

```bash
# 1. Clone the repository
git clone https://github.com/Asma-Shoukat/MediMate.git
cd MediMate

# 2. Open in Qt Creator
#    File → Open File or Project → select MediMate.pro

# 3. Select your Qt 6 kit and click Build (Ctrl+B)

# 4. Run (Ctrl+R)
```

> **Note:** The app auto-creates `profiles/`, `tasks/`, and `history/` directories on first run. No manual setup needed.

### First Use

1. Launch the app — you'll see the two-panel welcome screen
2. Click **Create New Profile** and fill in your health details
3. Your profile is saved locally and loaded automatically next time
4. Start chatting — try typing `I have a headache`, `recommend diet`, or `health score`

---

## 💡 How It Works

### Talking to the Bot

| What you type | What happens |
|---|---|
| `hello` / `hi` | Personalised greeting with your name |
| `I have a headache` | Starts symptom analysis → asks severity (1–10) → asks follow-ups → generates diagnosis card |
| `recommend diet` | Generates full 3-meal plan based on your health goal |
| `recommend exercise` | Generates structured workout routine based on your goal & duration |
| `health score` | Shows your live composite wellness score (0–100) with breakdown |
| `tasks` | Shows your task checklist with progress counter |
| `profile` | Displays your health profile card |
| `help` | Shows all available commands |
| `bye` | Farewell with a randomised wellness tip |

### Suggestion Chips
Clickable quick-action buttons appear below the chat that adapt based on the current conversation state — no typing needed.

### Severity Slider
When the bot asks about pain severity (1–10), a slider widget appears in the UI for precise input. It hides automatically after submission.

---

## 🛠️ Tech Stack

| Layer | Technology |
|---|---|
| Language | C++17 |
| UI Framework | Qt 6 (QtWidgets) |
| UI Design | Qt Style Sheets (QSS) |
| Data Persistence | `std::filesystem` + plain text |
| Random Engine | `std::mt19937` (Mersenne Twister) |
| Architecture | MVC + Finite State Machine |
| Build System | `qmake` (`.pro` file) |

---

## 👩‍💻 Author

<div align="center">

**Asma Shoukat**

[![GitHub](https://img.shields.io/badge/GitHub-Asma--Shoukat-181717?style=for-the-badge&logo=github)](https://github.com/Asma-Shoukat)

</div>

---

## ⚠️ Disclaimer

MediMate AI is an **educational project** and is **not a substitute for professional medical advice, diagnosis, or treatment**. Always seek the guidance of a qualified healthcare provider with any questions you may have regarding a medical condition. In case of a medical emergency, call your local emergency services immediately.

---

<div align="center">

Made with ❤️ using **Qt 6** & **C++17**

<br>

*"Small daily habits lead to extraordinary results."*

</div>
