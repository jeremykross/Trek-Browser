#ifndef SIGNUPWIZARD_H
#define SIGNUPWIZARD_H
#include <QObject>
#include <QString>
#include <QWizard>
#include <QWizardPage>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QInputDialog>
#include <QKeyEvent>
#include <QCalendarWidget>

#include "SwitchWidget.h"
#include "SlideTransition.h"
#include "InfiniteProgressWidget.h"
#include "Controller.h"
#include "AvatarDesignWidget.h"
#include "UserData.h"

class SignupWaitPage: public QWizardPage {
	Q_OBJECT
	public:
		SignupWaitPage(QWidget* parent=NULL);
		void initializePage();
		bool isComplete() const;
	signals:
		void connectWithServer();
		void signup(const QString& emailAddress, const QString& firstName, const QString& lastName, const QString& sex,const QDate& birthDate, const QString& userName, const QString& password, const QString& authKey); 
		void authenticate(const QString& username, const QString& password); 
		void controller_serverRequestPasswordReset();

	protected slots:
		void connected();
		void failedToConnect();
		void signupSuccess();
		void signupFailure(const QString& mesg);
		void authenticationSuccess(UserData* data);
		void authenticationFailure();
	protected:
		void initialize();
		QVBoxLayout mMainLayout;
		SwitchWidget mSwitchWidget;
		QLabel mMessageLabel;
		InfiniteProgressWidget mProgressWidget;

		bool mNeedAction;
		bool mIsComplete;
	
};

class SignupIntroPage: public QWizardPage {
	public:
		SignupIntroPage(QWidget* parent=NULL);
	protected:	
		void initialize();
		QVBoxLayout mMainLayout;
};

class SignupRequiredInfoPage: public QWizardPage {
	Q_OBJECT
	public:
		SignupRequiredInfoPage(QWidget* parent=NULL);
		bool isComplete() const;
	protected slots:
		void checkPasswordEquality();
		void checkUsernameValidity();
		void checkDateValidity();
	protected:
		void initialize();
		//QVBoxLayout mMainLayout;
		QFormLayout mMainLayout;
		QLineEdit mEmailAddressLineEdit;
		QLineEdit mFirstNameLineEdit;
		QLineEdit mLastNameLineEdit;
		QComboBox mSexComboBox;
		QCalendarWidget mCalendar;
		QLineEdit mUsernameLineEdit;
		QLineEdit mPasswordLineEdit;
		QLineEdit mPWCheckLineEdit;
		bool mPasswordEqual;
		bool mUsernameValid;
		bool mDateValid;
};

class SignupAvatarDesignPage: public QWizardPage {
	Q_OBJECT
	public:
		SignupAvatarDesignPage(QWidget* parent=NULL);
		~SignupAvatarDesignPage();
		bool validatePage();
	signals:
		void disconnect();
	protected:
		void initialize();
		QVBoxLayout mMainLayout;
		AvatarDesignAndTestWidget* mDesignWidget;

};

class SignupWizard: public QWizard {
	Q_OBJECT
	public:
		SignupWizard(QWidget* parent=NULL);
	protected:
		void keyPressEvent(QKeyEvent* evt);
		void initialize();

};

#endif
