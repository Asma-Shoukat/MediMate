#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Debug: Show current working directory
    qDebug() << "Current working directory:" << QDir::currentPath();

    bool stylesheetLoaded = false;

    // Try Method 1: Load from Qt Resources (recommended for deployment)
    QFile resourceFile(":/stylesheet.qss");
    if (resourceFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(resourceFile.readAll());
        a.setStyleSheet(styleSheet);
        resourceFile.close();
        qDebug() << "Stylesheet loaded successfully from Qt Resources.";
        stylesheetLoaded = true;
    } else {
        qDebug() << "Could not load stylesheet from Qt Resources. Trying alternative methods...";
    }

    // Try Method 2: Load from current directory (if resources failed)
    if (!stylesheetLoaded) {
        QFile currentDirFile("stylesheet.qss");
        if (currentDirFile.open(QFile::ReadOnly | QFile::Text)) {
            QString styleSheet = QLatin1String(currentDirFile.readAll());
            a.setStyleSheet(styleSheet);
            currentDirFile.close();
            qDebug() << "Stylesheet loaded successfully from current directory.";
            stylesheetLoaded = true;
        } else {
            qDebug() << "Could not load stylesheet from current directory.";
        }
    }

    // Try Method 3: Load from project source directory (relative path)
    if (!stylesheetLoaded) {
        QFile sourceFile("../../../stylesheet.qss"); // Adjust path as needed
        if (sourceFile.open(QFile::ReadOnly | QFile::Text)) {
            QString styleSheet = QLatin1String(sourceFile.readAll());
            a.setStyleSheet(styleSheet);
            sourceFile.close();
            qDebug() << "Stylesheet loaded successfully from source directory.";
            stylesheetLoaded = true;
        } else {
            qDebug() << "Could not load stylesheet from source directory.";
        }
    }

    // Final fallback message
    if (!stylesheetLoaded) {
        qWarning() << "WARNING: Could not load stylesheet from any location. Using default Qt styles.";
        qWarning() << "To fix this:";
        qWarning() << "1. Copy stylesheet.qss to the same folder as your executable, OR";
        qWarning() << "2. Add stylesheet.qss to your resources.qrc file and rebuild the project";
    }

    MainWindow w;
    w.setWindowTitle("MediMate - Your Health Companion");
    w.show();

    return a.exec();
}
