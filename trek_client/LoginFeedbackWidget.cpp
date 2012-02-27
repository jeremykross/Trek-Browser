#include "LoginFeedbackWidget.h"
	
LoginFeedbackWidget::LoginFeedbackWidget(QWidget* parent):SwitchWidget(parent) {
	mAllowedData = NULL;

	setWindowTitle("Trek Login");
	addWidget(&mLoginWidget);
	addWidget(&mProgressWidget);
//	addWidget(&mKeyWidget);
	addWidget(&mPWChangeWidget);
	addWidget(&mPWResetWidget);
	addWidget(&mReasonLabel);

	setNextWidget(&mProgressWidget, new SlideTransition(false, UP, 400));

	mReasonLabel.setWordWrap(true);

	connect(&mLoginWidget, SIGNAL(serverConnectionFailure()), this, SLOT(failedToConnect()));
	connect(&mLoginWidget, SIGNAL(wait()), this, SLOT(wait()));
	connect(&mLoginWidget, SIGNAL(carryOn()), this, SLOT(returnToForm()));
	connect(&mLoginWidget, SIGNAL(denied(const QString&)), this, SLOT(denied(const QString&)));
	connect(&mLoginWidget, SIGNAL(rejected()), this, SLOT(close()));
	connect(&mLoginWidget, SIGNAL(allowed(UserData*)), this, SLOT(gotAllowed(UserData*)));
	//connect(Controller::self(), SIGNAL(requestConfirmationKey()), this, SLOT(controller_requestConfirmationKey()));
	//connect(Controller::self(), SIGNAL(confKeyValid(bool)), this, SLOT(controller_confKeyValid(bool)));
	connect(&mPWResetWidget, SIGNAL(controller_serverRequestPasswordReset()), this, SLOT(serverRequestPasswordReset()));

	resize(425,0);
	setVisible(false);
};

void LoginFeedbackWidget::gotAllowed(UserData* data) {
	mAllowedData = data;
	if(mLoginWidget.password().startsWith("reset_")) {
		setCurrentWidget(&mPWChangeWidget, new SlideTransition(false, LEFT, 400));
		connect(&mPWChangeWidget, SIGNAL(controller_serverChangePassword(const QString&)), this, SLOT(doAllow()));
	} else {
		QTimer::singleShot(400, this, SLOT(doAllow()));
	}
}
void LoginFeedbackWidget::doAllow() {
	if(mAllowedData) {
		emit allowed(mAllowedData);
	}
}

void LoginFeedbackWidget::wait() {
	setCurrentWidget(&mProgressWidget, new SlideTransition(false, UP, 400));
}

void LoginFeedbackWidget::returnToForm() {
	setCurrentWidget(&mLoginWidget, new SlideTransition(false, RIGHT, 400));	
}

void LoginFeedbackWidget::flashMessage(const QString& message, int duration) {
	QFont f = mReasonLabel.font();
	f.setPointSize(14);
	mReasonLabel.setFont(f);
	mReasonLabel.setText(message); //"<font color=red>Server Says:</font> <br \\> " + reason);
	mReasonLabel.setAlignment(Qt::AlignCenter);
	setCurrentWidget(&mReasonLabel, new SlideTransition(false, LEFT, 400));
	QTimer::singleShot(duration, this, SLOT(returnToForm()));
}

void LoginFeedbackWidget::denied(const QString& reason) {
	if(reason.contains("Password")) {
		setCurrentWidget(&mPWResetWidget, new SlideTransition(false, LEFT, 400));
		connect(&mPWResetWidget, SIGNAL(canceled()), this, SLOT(returnToForm()));
	} else {
		flashMessage(SERVER_SAYS + reason);
	}
}

void LoginFeedbackWidget::failedToConnect() {
	flashMessage("Trek couldn't connect to the server.");
}

/*void LoginFeedbackWidget::controller_requestConfirmationKey() {
	setCurrentWidget(&mKeyWidget, new SlideTransition(false, LEFT, 400));
	connect(&mKeyWidget, SIGNAL(canceled()), this, SLOT(returnToForm()));
	connect(&mKeyWidget, SIGNAL(authenticationSent()), this, SLOT(wait()));
	mLoginWidget.unreadyAuthentication();
	
}*/
/*void LoginFeedbackWidget::controller_confKeyValid(bool isValid) {
	if(isValid) {
		mLoginWidget.readyAuthentication();
	//	flashMessage("The Email Confirmation Key Appears to Be Valid.  Thanks!");	
	} else {
		flashMessage("The Email Confirmation Key is Invalid.");
	}
}*/
void LoginFeedbackWidget::serverRequestPasswordReset() {
	//emit controller_serverRequestPasswordReset(mLoginWidget.emailAddress()); 
	flashMessage("A New Password Has Been Emailed to You.");
}
