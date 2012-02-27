#include "SignupDataWidget.h"

SignupDataWidget::SignupDataWidget(QWidget* parent):QWidget(parent) {
	initialize();
}

void SignupDataWidget::initialize() {
	setWindowTitle("Pluto - Signup");

	mFormLayout.addRow(new QLabel("*Privacy will always be respected! We hate spam too."));
	mFormLayout.addRow("Email Address: ", &mEmailAddressLineEdit);

	mFormLayout.addRow("First name: ", &mFirstNameLineEdit);
	mFormLayout.addRow("Last name: ", &mLastNameLineEdit);	
	mSexComboBox.addItem("Male");
	mSexComboBox.addItem("Female");
	mSexComboBox.addItem("Other?");
	mSexComboBox.setCurrentIndex(-1);
	mFormLayout.addRow("Sex: ", &mSexComboBox);
	
	mFormLayout.addRow(new QLabel(""));

	mFormLayout.addRow("Desired Username: ", &mUsernameLineEdit);
	mPasswordLineEdit.setEchoMode(QLineEdit::Password);
	mFormLayout.addRow("Password: ", &mPasswordLineEdit);
	mPasswordCheck.setEchoMode(QLineEdit::Password);
	mFormLayout.addRow("Retype Password: ",&mPasswordCheck); 

	mSubmitButton.setText("Submit");
	mButtonLayout.addWidget(&mSubmitButton);
	
	mFormLayout.addRow(&mButtonLayout);

	setLayout(&mFormLayout);

	connect(&mSubmitButton, SIGNAL(clicked()), this, SLOT(submit()));
	connect(this, SIGNAL(attemptSignup(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)), Controller::self(), SIGNAL(serverSignupAttempted(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&)));
//	setCentralLayout(&mFormLayout);
}

void SignupDataWidget::submit() {
	emit attemptSignup(mEmailAddressLineEdit.text(), mFirstNameLineEdit.text(), mLastNameLineEdit.text(), mSexComboBox.currentText(), mUsernameLineEdit.text(), mPasswordLineEdit.text());
}
