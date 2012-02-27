#include "SignupWizard.h"

SignupWaitPage::SignupWaitPage(QWidget* parent):QWizardPage(parent) {
	initialize();
}
void SignupWaitPage::initialize() {
	setTitle("Please Wait...");
	setSubTitle("We're checking with the server to make sure your input are okay.");
	setCommitPage(true);
	
	mMessageLabel.setWordWrap(true);
	mMessageLabel.setAlignment(Qt::AlignCenter);
	QFont f;
	f.setPointSize(14);
	mMessageLabel.setFont(f);

	mSwitchWidget.addWidget(&mProgressWidget);
	mSwitchWidget.addWidget(&mMessageLabel);
	mMainLayout.addWidget(&mSwitchWidget);

	mProgressWidget.setMinimumSize(100,100);
	setLayout(&mMainLayout);
	
	connect(this, SIGNAL(signup(const QString&, const QString&, const QString&, const QString&,const QDate&, const QString&, const QString&, const QString&)), Controller::self(), SIGNAL(serverSignupAttempted(const QString&, const QString&, const QString&, const QString&,const QDate&, const QString&, const QString&,const QString&)));
	connect(this, SIGNAL(connectWithServer()), Controller::self(), SIGNAL(serverConnectionAttempted()));
	connect(Controller::self(), SIGNAL(serverConnected()), this, SLOT(connected()));
	connect(Controller::self(), SIGNAL(serverSignupSuccess()), this, SLOT(signupSuccess())); 
	connect(Controller::self(), SIGNAL(serverSignupFailure(const QString&)), this, SLOT(signupFailure(const QString&)));
	connect(Controller::self(), SIGNAL(serverConnectionFailure()), this, SLOT(failedToConnect()));

	connect(this, SIGNAL(authenticate(const QString&, const QString&)), Controller::self(), SIGNAL(serverAuthenticationAttempted(const QString&, const QString&)));
	connect(this, SIGNAL(controller_serverRequestPasswordReset()), Controller::self(), SIGNAL(serverRequestPasswordReset()));

	connect(Controller::self(), SIGNAL(serverAuthenticationSuccess(UserData*)), this, SLOT(authenticationSuccess(UserData*)));
	connect(Controller::self(), SIGNAL(serverAuthenticationFailure(const QString&)), this, SLOT(authenticationFailure()));

	//connect(Controller::self(), SIGNAL(serverSignupFailed(const QString& reason)),

	mNeedAction = false;
}
	
void SignupWaitPage::connected() {
	//mProgressWidget.stop();
	if(mNeedAction==false) return;

	//Do the action
	mNeedAction = false;
	//QString key = QInputDialog::getText(this, "Please Input Your Beta Key", "If you have a key, it should be in your email inbox.");
	QString key;
	emit signup(field("emailAddress").toString(), field("firstName").toString(), field("lastName").toString(), field("sex").toString(),field("birthDate").toDate(), field("userName").toString(), field("password").toString(), key);
}
void SignupWaitPage::initializePage() {
	mIsComplete = false;
	emit completeChanged();
	mNeedAction = true;
	emit connectWithServer();

	mProgressWidget.stop();
	mSwitchWidget.setCurrentWidget(&mProgressWidget);
	mProgressWidget.start();
}
void SignupWaitPage::signupSuccess() {
	//QMessageBox::information(this, "Congradulations", "Signup was successful!");
	mMessageLabel.setText("Signup was successful!\nAn email has been sent with your temporary password.");
	mProgressWidget.stop();
	mSwitchWidget.setCurrentWidget(&mMessageLabel, new SlideTransition(false, UP, 500));
	
	emit authenticate(field("emailAddress").toString(), field("password").toString());

	mIsComplete = true;
	emit completeChanged();
}

void SignupWaitPage::signupFailure(const QString& reasonMesg) {
	//QMessageBox::information(this, "Oops!", "Signup Failed.  Please go back and double check your input.");
	mMessageLabel.setText("<p>Signup Failed! <br/><br/> <font color=red> Server Says:</font>" +  reasonMesg + "</p>");
	mSwitchWidget.setCurrentWidget(&mMessageLabel, new SlideTransition(false, UP, 500));
	QTimer::singleShot(3500, wizard(), SLOT(back()));
}
void SignupWaitPage::failedToConnect() {
	mMessageLabel.setText("<p>Signup Failed! <br/><br/> Couldn't Connect to the Server!");
	mSwitchWidget.setCurrentWidget(&mMessageLabel, new SlideTransition(false, UP, 500));
	QTimer::singleShot(3500, wizard(), SLOT(back()));
}

void SignupWaitPage::authenticationSuccess(UserData*) {
	//QMessageBox::information(NULL, "Signup Wait Page", "Authentication Success!");
}
void SignupWaitPage::authenticationFailure() {
	//QMessageBox::information(NULL, "Signup Wait Page", "Authentication Failure!");
}

bool SignupWaitPage::isComplete() const {
	return mIsComplete;
}

SignupIntroPage::SignupIntroPage(QWidget*) {
	initialize();
}

void SignupIntroPage::initialize() {
	setTitle("Hello..");
//	setSubTitle("This Assistant will guide you through the process of signing up for Pluto.");
	QLabel* label = new QLabel();
	label->setText("..and Welcome! This assistant will guide you through the process of signing up for Trek.");
	label->setWordWrap(true);
	mMainLayout.addWidget(label);
	setLayout(&mMainLayout);
}

SignupRequiredInfoPage::SignupRequiredInfoPage(QWidget* parent):QWizardPage(parent) {
	mPasswordEqual = true; 
	mUsernameValid = false;
	mDateValid = false;
	initialize();
}
void SignupRequiredInfoPage::initialize() {
	setTitle("Needed Information");
	setCommitPage(true);
	QLabel* label = new QLabel();
	label->setText("Spam sucks. Everything will be kept confidential.");
	label->setWordWrap(true);
	mPasswordLineEdit.setEchoMode(QLineEdit::Password);
	mPWCheckLineEdit.setEchoMode(QLineEdit::Password);

	connect(&mUsernameLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkUsernameValidity()));	
	//connect(&mPasswordLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkPasswordEquality()));
	//connect(&mPWCheckLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(checkPasswordEquality()));

	connect(&mCalendar, SIGNAL(selectionChanged()), this, SLOT(checkDateValidity()));

	mSexComboBox.addItem("Female");
	mSexComboBox.addItem("Male");
	mSexComboBox.addItem("Other?");
	mSexComboBox.setCurrentIndex(-1);
	connect(&mSexComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));

	//mMainLayout.setRowWrapPolicy(QFormLayout::WrapLongRows);
	mMainLayout.addRow(label);
	mMainLayout.addRow(new QLabel());
	mMainLayout.addRow("Desired Username:", &mUsernameLineEdit);
	mMainLayout.addRow(new QLabel());
	mMainLayout.addRow("Email Address:", &mEmailAddressLineEdit);
	mMainLayout.addRow("First Name:", &mFirstNameLineEdit);
	mMainLayout.addRow("Last Name:", &mLastNameLineEdit);
	mMainLayout.addRow("Sex:", &mSexComboBox);
	mMainLayout.addRow("Birth Date (year/m/d):", &mCalendar); 
	mMainLayout.addRow(new QLabel());
	//mMainLayout.addRow("Password:", &mPasswordLineEdit);
	//mMainLayout.addRow("Password (again to doublecheck):", &mPWCheckLineEdit);
	setLayout(&mMainLayout);

	registerField("emailAddress*", &mEmailAddressLineEdit);
	registerField("firstName*", &mFirstNameLineEdit);
	registerField("lastName*", &mLastNameLineEdit);
	registerField("userName*", &mUsernameLineEdit);
	registerField("sex*", &mSexComboBox, "currentText");
	registerField("birthDate", &mCalendar, "selectedDate");
	registerField("password", &mPasswordLineEdit);
	registerField("pwCheck", &mPWCheckLineEdit);
	mCalendar.setSelectedDate(QDate());
	//mCalendar.setMaximumDate(QDate::currentDate());
}

void SignupRequiredInfoPage::checkDateValidity() {
	if(mCalendar.selectedDate() == QDate::currentDate()) {
		return;
	}
	bool oldValidity = mDateValid;
	if(mCalendar.selectedDate() > QDate::currentDate()) {
		QMessageBox::information(NULL, "Date Descrepency", "You were born in the future?");
		mDateValid = false;
	} else if(mCalendar.selectedDate().year() == QDate::currentDate().year()) {
		QMessageBox::information(NULL, "Date Descrepency", "Did you remember to change the year of your birth?");
		mDateValid = false;
	} else {
		mDateValid = true;
	}
	if(mDateValid != oldValidity) {
		emit completeChanged();
	}
}
	
void SignupRequiredInfoPage::checkUsernameValidity() {
	bool oldValidity = mUsernameValid;	
	if(mUsernameLineEdit.text().count() > 20) {
		QMessageBox::information(this, "Sorry", "Usernames must be 20 characters or less.");
		QString truncated = mUsernameLineEdit.text();
		truncated.resize(20);
		mUsernameLineEdit.setText(truncated);
		mUsernameValid = false;
	} else {
		mUsernameValid = true;	
	}
	if(mUsernameValid != oldValidity) {
		std::cout << "Complete Changed!\n";
		emit completeChanged();		
	}
}

void SignupRequiredInfoPage::checkPasswordEquality() {
	bool oldEquality = mPasswordEqual;
	if(mPasswordLineEdit.text() == mPWCheckLineEdit.text()) {
		mPasswordEqual = true;	
	} else {
		mPasswordEqual = false;
	}
	if(mPasswordEqual != oldEquality) emit completeChanged();

}

bool SignupRequiredInfoPage::isComplete() const {
	/*if(mCalendar.selectedDate() != QDate::currentDate()) {
		if(mCalendar.selectedDate().year() == QDate::currentDate().year()) {
			QMessageBox::information(NULL, "Check Again.", "Did you remember to change to the year of your birthdate?");
			return false;
		} else if(mCalendar.selectedDate() > QDate::currentDate()) {
			QMessageBox::information(NULL, "Date Descrepency", "Alright, John Titur.  (You were born in the future?)");	
			return false;
		}
	}*/
	return mUsernameValid && mPasswordEqual && mDateValid && QWizardPage::isComplete();
}

SignupAvatarDesignPage::SignupAvatarDesignPage(QWidget* parent):QWizardPage(parent) {
	initialize();
}
SignupAvatarDesignPage::~SignupAvatarDesignPage() {
	/*	if(mDesignWidget) {
		mDesignWidget->deleteLater();
		}*/
}

void SignupAvatarDesignPage::initialize() {
	setTitle("Please Customize Your Avatar");
	mDesignWidget = new AvatarDesignAndTestWidget();
	mMainLayout.addWidget(mDesignWidget);
	setLayout(&mMainLayout);
	connect(this, SIGNAL(disconnect()), Controller::self(), SIGNAL(serverDisconnect()));
}
bool SignupAvatarDesignPage::validatePage() {
	mDesignWidget->designWidget()->commit();
	emit disconnect();

	return true;
}


SignupWizard::SignupWizard(QWidget* parent):QWizard(parent) {
	initialize();
}
void SignupWizard::initialize() {
	setWindowTitle("Signup Assistant");
	setOption(QWizard::NoDefaultButton, true);
	addPage(new SignupIntroPage);
	addPage(new SignupRequiredInfoPage);
	addPage(new SignupWaitPage);
	addPage(new SignupAvatarDesignPage);
}

void SignupWizard::keyPressEvent(QKeyEvent*) {

}
