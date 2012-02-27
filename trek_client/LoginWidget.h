#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

/* Core */
#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QObject>
#include <QString>

/* Gui */
#include <QFormLayout>
#include <QStackedLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QSizePolicy>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

/* Painting */
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QImage>

/* Std */
#include <iostream>


#include "Controller.h"
#include "MessageBlocks.h"
#include "SwitchWidget.h"
#include "SignupWizard.h"

//#define ICON_SIZE 128

class LoginWidget: public QDialog {
	Q_OBJECT
	public:
		LoginWidget(QWidget* parent = 0);
		~LoginWidget();
		void readyAuthentication();
		void unreadyAuthentication();
		QString emailAddress();
		QString password();
	signals:
		void serverConnectionFailure();
		void allowed(UserData* myUserData);
		void denied(const QString& reason);
		void connectWithServer();
		void authenticateWithServer(const QString& username, const QString& password);
		void wait();
		void carryOn();
	protected slots:
		void signup();
                void failedToConnect();
		void connected();
		void authenticationFailed();
		void send();
		void enable();
		void disable();
	protected:
		void initialize();
		QDialogButtonBox mButtons;

		QVBoxLayout mIconLayout;
		QVBoxLayout mMainLayout;
		QFormLayout mInputLayout;
		QLineEdit mUsernameLineEdit;
		QLineEdit mPasswordLineEdit;
		QPushButton mSendButton;
		QPushButton mCancelButton;
		QPushButton mSignupButton;
		QCheckBox mRememberPwdCheck;
		
		SignupWizard* mWiz;

		bool connectionActionNeeded;
};

#endif
