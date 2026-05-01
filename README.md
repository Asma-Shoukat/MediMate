<div align="center">

<img src="https://img.shields.io/badge/Qt-6.x-41CD52?style=for-the-badge&logo=qt&logoColor=white"/>
<img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white"/>
<img src="https://img.shields.io/badge/Platform-Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white"/>

# 🩺 MediMate — AI Health Assistant

**A professional, offline AI-powered health companion built with Qt 6 & C++17.**  
MediMate helps you track symptoms, get personalised diet & exercise plans, monitor your health score, and manage wellness tasks — all from a stunning desktop interface.

</div>

---

## ✨ Features

| Feature | Description |
|---|---|
| 🧠 **Symptom Analyser** | Conversational symptom checker with severity assessment & diagnosis |
| 🚨 **Emergency Detection** | Instant alerts for life-threatening symptom keywords |
| 🍽️ **Diet Planner** | Goal-aware personalised meal plans (muscle, weight loss, wellness) |
| 🏋️ **Exercise Coach** | Structured workout routines tailored to your health goal |
| 📊 **Health Score** | Real-time composite wellness score based on profile & task data |
| ✅ **Task Manager** | Trackable to-do list with health suggestions auto-added |
| 👤 **Multi-Profile** | Create, load, and delete user profiles with local persistence |
| 💾 **Session History** | Chat history saved per user session |
| 🎨 **Premium Light UI** | Modern two-panel design with Royal Blue accents and dark accents |

---

## 🖥️ Screenshots

> *Run the app in Qt Creator to see the live interface.*

---

## 🚀 Getting Started

### Prerequisites
- **Qt 6.x** (Qt Creator recommended)
- **C++17** compatible compiler (MSVC, GCC, or Clang)
- Windows / Linux / macOS

### Build Instructions

1. **Clone the repository**
   ```bash
   git clone https://github.com/Asma-Shoukat/MediMate.git
   cd MediMate
   ```

2. **Open in Qt Creator**
   - Open `MediMate.pro` with Qt Creator
   - Select your Qt 6 kit
   - Click **Build → Run** (`Ctrl+R`)

3. **First Run**
   - The app will auto-create `profiles/`, `tasks/`, and `history/` directories
   - Click **Create New Profile** to get started

---

## 🗂️ Project Structure

```
MediMate/
├── core_logic/
│   ├── chatbot/          # Conversational AI engine (state machine)
│   ├── managers/         # ProfileManager, ToDoManager, HistoryManager
│   ├── models/           # UserProfile, HealthScore, HealthTask, DietTask...
│   └── persistence/      # FileManager for save/load
├── resources/            # Icons and assets
├── mainwindow.cpp/.h     # Main application UI
├── stylesheet.qss        # Premium Light Theme
└── MediMate.pro          # Qt project file
```

---

## 🤖 How the Chatbot Works

MediMate uses a **state-machine architecture** to manage conversations:

```
IDLE → SEVERITY_ASSESSMENT → SYMPTOM_GATHERING → DIAGNOSIS → IDLE
         ↓
    EMERGENCY_DETECTED (interrupts at any state)
```

- Type a symptom → bot asks clarifying questions → generates a diagnosis card
- Type `recommend diet` → goal-aware meal plan generated
- Type `health score` → real-time wellness dashboard
- Click suggestion chips for quick actions

---

## 🛠️ Tech Stack

- **Language:** C++17
- **Framework:** Qt 6 (Widgets)
- **Persistence:** Plain text files via `std::filesystem`
- **Architecture:** MVC-inspired with state-machine chatbot
- **Styling:** Qt Style Sheets (QSS) — Premium Light Theme

---

## 👩‍💻 Author

**Asma Shoukat**  
[GitHub](https://github.com/Asma-Shoukat) 

---

## ⚠️ Disclaimer

MediMate is an educational AI assistant and **not a substitute for professional medical advice**. Always consult a qualified healthcare provider for medical decisions.

---

<div align="center">
  Made with ❤️ using Qt & C++
</div>
