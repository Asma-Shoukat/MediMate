// File: profiledetailsdialog.cpp (UI Entirely in C++ Code - CORRECTED)
#include "profiledetailsdialog.h"
#include "core_logic/models/userprofile.h" // For UserProfile class

#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QPushButton> // For getting standard button roles

// Constructor for creating a new profile
ProfileDetailsDialog::ProfileDetailsDialog(const QString& initialName, QWidget *parent) :
    QDialog(parent),
    nameLineEdit(nullptr), ageLineEdit(nullptr), genderLineEdit(nullptr),
    weightLineEdit(nullptr), heightLineEdit(nullptr), conditionsLineEdit(nullptr),
    goalLineEdit(nullptr), exerciseDurationLineEdit(nullptr), buttonBox(nullptr),
    editingProfile(nullptr)
{
    setupUiElements(); // Call helper to create UI
    setWindowTitle("Enter Profile Details");

    if (nameLineEdit && !initialName.isEmpty()) {
        nameLineEdit->setText(initialName);
        // If an initial name is provided (e.g., from QInputDialog),
        // it's often treated as the unique ID and shouldn't be changed in this dialog.
        nameLineEdit->setReadOnly(true);
    } else if (nameLineEdit) {
        nameLineEdit->setReadOnly(false); // Allow editing if no initial name
    }
}

// Constructor for editing an existing profile
ProfileDetailsDialog::ProfileDetailsDialog(UserProfile *profileToEdit, QWidget *parent) :
    QDialog(parent),
    nameLineEdit(nullptr), ageLineEdit(nullptr), genderLineEdit(nullptr),
    weightLineEdit(nullptr), heightLineEdit(nullptr), conditionsLineEdit(nullptr),
    goalLineEdit(nullptr), exerciseDurationLineEdit(nullptr), buttonBox(nullptr),
    editingProfile(profileToEdit)
{
    setupUiElements();
    setWindowTitle("Edit Profile Details");

    if (editingProfile) {
        populateFields(editingProfile);
        if (nameLineEdit) {
            nameLineEdit->setReadOnly(true); // Profile name (identifier) usually not editable
        }
    }
}

ProfileDetailsDialog::~ProfileDetailsDialog()
{
    // No explicit 'delete' needed for UI elements like nameLineEdit, buttonBox, etc.
    // because they are QObjects and have been parented to 'this' (the QDialog)
    // or to layouts which are in turn parented to 'this'.
    // Qt's parent-child memory management will handle their deletion
    // when the ProfileDetailsDialog instance is deleted.
}

void ProfileDetailsDialog::setupUiElements() {
    // Create LineEdits and parent them to this dialog
    nameLineEdit = new QLineEdit(this);
    ageLineEdit = new QLineEdit(this);
    genderLineEdit = new QLineEdit(this);
    weightLineEdit = new QLineEdit(this);
    heightLineEdit = new QLineEdit(this);
    conditionsLineEdit = new QLineEdit(this);
    goalLineEdit = new QLineEdit(this);
    exerciseDurationLineEdit = new QLineEdit(this);

    // Set placeholders for clarity
    nameLineEdit->setPlaceholderText("e.g., JohnDoe (no spaces)");
    ageLineEdit->setPlaceholderText("e.g., 30");
    genderLineEdit->setPlaceholderText("e.g., Male, Female, Other");
    weightLineEdit->setPlaceholderText("e.g., 175.5");
    heightLineEdit->setPlaceholderText("e.g., 5'11\"");
    conditionsLineEdit->setPlaceholderText("e.g., Diabetes, None");
    goalLineEdit->setPlaceholderText("e.g., Weight loss, Build muscle");
    exerciseDurationLineEdit->setPlaceholderText("e.g., 30");


    // Set validators for numeric fields
    ageLineEdit->setValidator(new QIntValidator(1, 150, this));
    weightLineEdit->setValidator(new QDoubleValidator(0.0, 1500.0, 2, this)); // Max weight 1500, 2 decimals
    exerciseDurationLineEdit->setValidator(new QIntValidator(0, 1440, this)); // Max 24 hours in minutes

    // Create Form Layout and add rows
    QFormLayout *formLayout = new QFormLayout(); // No parent here, will be added to mainLayout
    formLayout->addRow(new QLabel("Profile Name:", this), nameLineEdit);
    formLayout->addRow(new QLabel("Age:", this), ageLineEdit);
    formLayout->addRow(new QLabel("Gender:", this), genderLineEdit);
    formLayout->addRow(new QLabel("Weight (lb):", this), weightLineEdit);
    formLayout->addRow(new QLabel("Height (e.g., 5'7\"):", this), heightLineEdit);
    formLayout->addRow(new QLabel("Health Conditions:", this), conditionsLineEdit);
    formLayout->addRow(new QLabel("Health Goal:", this), goalLineEdit);
    formLayout->addRow(new QLabel("Exercise Duration (mins):", this), exerciseDurationLineEdit);

    // Create Button Box
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    // Set object names for buttons if you want to style them or access them specifically
    // buttonBox->button(QDialogButtonBox::Ok)->setObjectName("okButton");
    // buttonBox->button(QDialogButtonBox::Cancel)->setObjectName("cancelButton");

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ProfileDetailsDialog::acceptAndValidate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Main Layout for the Dialog
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(15); // Add some space before buttons
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout); // Apply the main layout to the dialog itself
    setMinimumWidth(350); // Set a reasonable minimum width for the dialog
}


void ProfileDetailsDialog::populateFields(UserProfile *profile) {
    if (!profile || !nameLineEdit) { /* Basic check if UI is setup */ return; }
    nameLineEdit->setText(QString::fromStdString(profile->getName()));
    ageLineEdit->setText(QString::number(profile->getAge()));
    genderLineEdit->setText(QString::fromStdString(profile->getGender()));
    weightLineEdit->setText(QString::number(profile->getWeightLb()));
    heightLineEdit->setText(QString::fromStdString(profile->getHeightFtIn()));
    conditionsLineEdit->setText(QString::fromStdString(profile->getHealthConditions()));
    goalLineEdit->setText(QString::fromStdString(profile->getHealthGoal()));
    exerciseDurationLineEdit->setText(QString::number(profile->getPreferredExerciseDurationMins()));
}

// Getter implementations (access member QLineEdits directly)
QString ProfileDetailsDialog::getProfileName() const { return nameLineEdit ? nameLineEdit->text().trimmed() : ""; }
int ProfileDetailsDialog::getAge() const { bool ok; int val = ageLineEdit ? ageLineEdit->text().toInt(&ok) : 0; return ok ? val : -1; }
QString ProfileDetailsDialog::getGender() const { return genderLineEdit ? genderLineEdit->text().trimmed() : ""; }
double ProfileDetailsDialog::getWeightLb() const { bool ok; double val = weightLineEdit ? weightLineEdit->text().toDouble(&ok) : 0.0; return ok ? val : -1.0; }
QString ProfileDetailsDialog::getHeightFtIn() const { return heightLineEdit ? heightLineEdit->text().trimmed() : ""; }
QString ProfileDetailsDialog::getHealthConditions() const { return conditionsLineEdit ? conditionsLineEdit->text().trimmed() : ""; }
QString ProfileDetailsDialog::getHealthGoal() const { return goalLineEdit ? goalLineEdit->text().trimmed() : ""; }
int ProfileDetailsDialog::getPreferredExerciseDurationMins() const { bool ok; int val = exerciseDurationLineEdit ? exerciseDurationLineEdit->text().toInt(&ok) : 0; return ok ? val : -1; }

void ProfileDetailsDialog::applyToUserProfile(UserProfile* profile) {
    if (!profile) return;
    // Name is set based on initialName or if editing (usually name is read-only when editing)
    // If nameLineEdit ISN'T read-only and is meant to be set:
    if (nameLineEdit && !nameLineEdit->isReadOnly()) {
        profile->setName(getProfileName().toStdString());
    }

    profile->setAge(getAge());
    profile->setGender(getGender().toStdString());
    profile->setWeightLb(getWeightLb());
    profile->setHeightFtIn(getHeightFtIn().toStdString());
    profile->setHealthConditions(getHealthConditions().toStdString());
    profile->setHealthGoal(getHealthGoal().toStdString());
    profile->setPreferredExerciseDurationMins(getPreferredExerciseDurationMins());
}

bool ProfileDetailsDialog::validateInputs(QString& errorMessages) {
    errorMessages.clear(); bool valid = true;
    // Ensure UI elements are created before accessing them
    if (!nameLineEdit || !ageLineEdit || !genderLineEdit || !weightLineEdit ||
        !heightLineEdit || !conditionsLineEdit || !goalLineEdit || !exerciseDurationLineEdit ) {
        errorMessages = "Dialog UI elements not initialized correctly. Cannot validate.";
        return false;
    }

    if (nameLineEdit->text().trimmed().isEmpty() && !nameLineEdit->isReadOnly()) {
        errorMessages += "Profile Name cannot be empty.\n"; valid = false;
        if(valid) nameLineEdit->setFocus(); // Set focus on first error
    }
    bool conversionOk;
    int ageVal = ageLineEdit->text().toInt(&conversionOk);
    if (!conversionOk || ageVal <= 0 || ageVal > 150) {
        errorMessages += "Please enter a valid age (1-150).\n"; if(valid) ageLineEdit->setFocus(); valid = false;
    }
    double weightVal = weightLineEdit->text().toDouble(&conversionOk);
    if (!conversionOk || weightVal < 0) {
        errorMessages += "Please enter a valid weight (non-negative).\n"; if(valid) weightLineEdit->setFocus(); valid = false;
    }
    int durationVal = exerciseDurationLineEdit->text().toInt(&conversionOk);
    if (!conversionOk || durationVal < 0) {
        errorMessages += "Please enter a valid exercise duration (non-negative minutes).\n"; if(valid) exerciseDurationLineEdit->setFocus(); valid = false;
    }
    if (genderLineEdit->text().trimmed().isEmpty()){ errorMessages += "Gender cannot be empty.\n"; if(valid) genderLineEdit->setFocus(); valid = false; }
    if (heightLineEdit->text().trimmed().isEmpty()){ errorMessages += "Height cannot be empty.\n"; if(valid) heightLineEdit->setFocus(); valid = false; }
    // Conditions can be "None" or empty, so not making it mandatory
    // if (conditionsLineEdit->text().trimmed().isEmpty()){ errorMessages += "Health Conditions cannot be empty (enter 'None' if applicable).\n"; if(valid) conditionsLineEdit->setFocus(); valid = false; }
    if (goalLineEdit->text().trimmed().isEmpty()){ errorMessages += "Health Goal cannot be empty.\n"; if(valid) goalLineEdit->setFocus(); valid = false; }

    return valid;
}

void ProfileDetailsDialog::acceptAndValidate()
{
    QString errors;
    if (validateInputs(errors)) {
        if (editingProfile) {
            applyToUserProfile(editingProfile);
        }
        // For new profiles, MainWindow will call applyToUserProfile after accept()
        accept();
    } else {
        QMessageBox::warning(this, "Input Validation Error", errors);
    }
}
