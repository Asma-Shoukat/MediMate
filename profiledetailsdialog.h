// File: profiledetailsdialog.h (UI Entirely in C++ Code - CORRECTED)
#ifndef PROFILEDETAILSDIALOG_H
#define PROFILEDETAILSDIALOG_H

#include <QDialog>
#include <QString>

// Forward declarations for Qt widgets we will use as members
class QLineEdit;
class QLabel;
class QDialogButtonBox;
class QFormLayout;
class QVBoxLayout;

class UserProfile; // Forward declare

class ProfileDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor for creating a new profile (name might be pre-filled or set here)
    explicit ProfileDetailsDialog(const QString& initialName = "", QWidget *parent = nullptr);

    // Constructor for editing an existing profile
    explicit ProfileDetailsDialog(UserProfile *profileToEdit, QWidget *parent = nullptr);

    ~ProfileDetailsDialog(); // Explicitly declared destructor

    // Getter methods to retrieve data from the dialog's input fields
    QString getProfileName() const;
    int getAge() const;
    QString getGender() const;
    double getWeightLb() const;
    QString getHeightFtIn() const;
    QString getHealthConditions() const;
    QString getHealthGoal() const;
    int getPreferredExerciseDurationMins() const;

    // Helper to apply dialog data to a UserProfile object
    // Call this AFTER dialog is accepted and getters return valid data.
    void applyToUserProfile(UserProfile* profile);

private slots:
    void acceptAndValidate(); // Slot for OK/Save button action

private:
    // UI Element Member Pointers
    QLineEdit *nameLineEdit;
    QLineEdit *ageLineEdit;
    QLineEdit *genderLineEdit;
    QLineEdit *weightLineEdit;
    QLineEdit *heightLineEdit;
    QLineEdit *conditionsLineEdit;
    QLineEdit *goalLineEdit;
    QLineEdit *exerciseDurationLineEdit;
    QDialogButtonBox *buttonBox;

    UserProfile *editingProfile; // Pointer to profile being edited (nullptr if creating new)

    void setupUiElements(); // Helper to create and layout UI elements
    void populateFields(UserProfile *profile); // Helper to fill fields if editing
    bool validateInputs(QString& errorMessages); // Helper to validate user input
};

#endif // PROFILEDETAILSDIALOG_H
