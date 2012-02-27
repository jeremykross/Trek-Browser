#ifndef LOGINFEEDBACKWIDGET_H
#define LOGINFEEDBACKWIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QLabel>
#include <QTimer>
#include <QFont>

#include "SwitchWidget.h"
#include "LoginWidget.h"
//#include "ConfirmationKeyWidget.h"
#include "PasswordResetWidget.h"
#include "PasswordChangeWidget.h"
#include "InfiniteProgressWidget.h"

#include "SlideTransition.h"

#define SERVER_SAYS "<font color=red>Server Says:</font> <br \\> " 

class LoginFeedbackWidget: public SwitchWidget {
	Q_OBJECT
	public:	
		LoginFeedbackWidget(QWidget* parent=NULL);
	signals:
		void allowed(UserData*);
		void controller_serverRequestPasswordReset(const QString& emailAddy);
	protected slots:
		void gotAllowed(UserData*);
		void doAllow();
		void wait();
		void returnToForm();
		void denied(const QString& reason);
		void failedToConnect();
		//void controller_requestConfirmationKey();
	//	void controller_confKeyValid(bool valid);
		void serverRequestPasswordReset();
	protected:
		void flashMessage(const QString& message, int duration=2500); 
		UserData* mAllowedData;
		LoginWidget mLoginWidget;
		//ConfirmationKeyWidget mKeyWidget;
		PasswordResetWidget mPWResetWidget;
		PasswordChangeWidget mPWChangeWidget;
		//PasswordRecoveryWidget
		//EmailAuthenticationWidget
		InfiniteProgressWidget mProgressWidget;	
		QLabel mReasonLabel;
};

#endif
