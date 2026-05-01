QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets # This line is redundant if the above line has widgets, but harmless

CONFIG += c++17
# For C++17 filesystem support in ProfileManager with MinGW, you might need this if you get linker errors later:
# CONFIG += C++17 # Redundant if c++17 is already there
# win32-g++: LIBS += -lstdc++fs
# Or for general g++ on windows if the above doesn't work:
# win32: LIBS += -lstdc++fs

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    core_logic/chatbot/chatbot.cpp \
    core_logic/managers/profilemanager.cpp \
    core_logic/managers/todomanager.cpp \
    core_logic/managers/historymanager.cpp \
    core_logic/models/conversationentry.cpp \
    core_logic/models/healthscore.cpp \
    core_logic/models/diettask.cpp \
    core_logic/models/exercisetask.cpp \
    core_logic/models/generictask.cpp \
    core_logic/models/healthtask.cpp \
    core_logic/models/userprofile.cpp \
    core_logic/persistence/file_manager.cpp \
    profiledetailsdialog.cpp

HEADERS += \
    mainwindow.h \
    core_logic/chatbot/chatbot.h \
    core_logic/managers/profilemanager.h \
    core_logic/managers/todomanager.h \
    core_logic/managers/historymanager.h \
    core_logic/models/conversationentry.h \
    core_logic/models/healthscore.h \
    core_logic/models/diettask.h \
    core_logic/models/exercisetask.h \
    core_logic/models/generictask.h \
    core_logic/models/healthtask.h \
    core_logic/models/userprofile.h \
    core_logic/persistence/file_manager.h \
    profiledetailsdialog.h

FORMS += \
    # profiledetailsdialog.ui

# Default rules for deployment.
# These are usually fine and don't need modification unless you have specific deployment needs.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Console settings should now be fully commented out or removed
# For console testing of backend:
# CONFIG += console
# CONFIG -= app_bundle
# CONFIG -= qt

DISTFILES += \
    stylesheet.qss

RESOURCES += \
    resources.qrc
