#include "LoginWidget.h"

LoginWidget::LoginWidget(QWidget* parent):QDialog(parent) {	
	mWiz = NULL;
	initialize();	
}

LoginWidget::~LoginWidget() {
	if(mWiz) {
		//Look Here!
	//	mWiz->deleteLater();
		mWiz = NULL;
	}
}

void LoginWidget::initialize() {
	setWindowTitle("Login to Trek");

	connectionActionNeeded = false;

	mSignupButton.setText("Signup");

	mSendButton.setText("Login");
	mCancelButton.setText("Cancel");
	mButtons.setOrientation(Qt::Horizontal);
	mButtons.addButton(&mSignupButton, QDialogButtonBox::ResetRole);
	mButtons.addButton(&mSendButton, QDialogButtonBox::AcceptRole);
	mButtons.addButton(&mCancelButton, QDialogButtonBox::RejectRole);

	mPasswordLineEdit.setEchoMode(QLineEdit::Password);
	mInputLayout.addRow("Email Address: ", &mUsernameLineEdit);
	mInputLayout.addRow("Password: ", &mPasswordLineEdit);
	mMainLayout.addLayout(&mInputLayout);
	mMainLayout.addWidget(&mButtons);


	setMinimumWidth(324);

	QLabel* l = new QLabel();
	l->setPixmap(QPixmap(":/icons/pluto_icon.png").scaled(ICON_SIZE,ICON_SIZE,Qt::KeepAspectRatio, Qt::SmoothTransformation));
	l->setAlignment(Qt::AlignVCenter);
	mIconLayout.addWidget(l);
	mIconLayout.setAlignment(l, Qt::AlignCenter);
	mIconLayout.addLayout(&mMainLayout);
	//setMaximumHeight(0);
	setLayout(&mIconLayout);

	connect(&mButtons, SIGNAL(accepted()), this, SLOT(send()));
	connect(&mButtons, SIGNAL(rejected()), this, SLOT(close()));
	connect(&mSignupButton, SIGNAL(clicked()), this, SLOT(signup()));

	connect(this, SIGNAL(allowed(UserData*)), this, SLOT(enable()));
        connect(this, SIGNAL(denied(const QString&)), this, SLOT(authenticationFailed()));
	connect(Controller::self(), SIGNAL(serverDisconnected()), this, SLOT(enable()));
        connect(Controller::self(), SIGNAL(serverConnectionFailure()), this, SLOT(failedToConnect()));
	connect(Controller::self(), SIGNAL(serverConnected()), this, SLOT(connected())); 
	connect(this, SIGNAL(connectWithServer()), Controller::self(), SIGNAL(serverConnectionAttempted()));
	connect(this, SIGNAL(authenticateWithServer(const QString&, const QString&)), Controller::self(), SIGNAL(serverAuthenticationAttempted(const QString&, const QString&)));
	enable();
}

void LoginWidget::signup() {	
	//if(mWiz) delete mWiz;
	//mWiz->deleteLater();
	mWiz = new SignupWizard();
	mWiz->setAttribute(Qt::WA_DeleteOnClose);
	mWiz->show();
}

void LoginWidget::enable() {
	setEnabled(true);
}
void LoginWidget::disable() {
	setEnabled(false);
}
void LoginWidget::failedToConnect() {
	emit serverConnectionFailure();
}
void LoginWidget::readyAuthentication() {
		connect(Controller::self(), SIGNAL(serverAuthenticationSuccess(UserData*)), this, SIGNAL(allowed(UserData*)));
		connect(Controller::self(), SIGNAL(serverAuthenticationFailure(const QString&)), this, SIGNAL(denied(const QString&)));

}
void LoginWidget::unreadyAuthentication() {
	disconnect(Controller::self(), SIGNAL(serverAuthenticationSuccess(UserData*)), this, SIGNAL(allowed(UserData*)));
	disconnect(Controller::self(), SIGNAL(serverAuthenticationFailure(const QString&)), this, SIGNAL(denied(const QString&)));
}

QString LoginWidget::emailAddress() {
	return mUsernameLineEdit.text();
}
QString LoginWidget::password() {
	return mPasswordLineEdit.text().trimmed();
}
void LoginWidget::connected() {
	if(connectionActionNeeded) {
		connectionActionNeeded = false;
		readyAuthentication();
		emit authenticateWithServer(mUsernameLineEdit.text(), mPasswordLineEdit.text().trimmed());
	}
}
void LoginWidget::authenticationFailed() {
//	QMessageBox::information(this, "Authentication Failed", "Your username and password could not be verified.");
	unreadyAuthentication();
	emit carryOn();
}


void LoginWidget::send() {
	connectionActionNeeded = true;
        emit connectWithServer();
	emit wait();
}


