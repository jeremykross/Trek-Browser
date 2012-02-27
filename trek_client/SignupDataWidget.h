#ifndef SIGNUPDATAWIDGET_H
#define SIGNUPDATAWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QObject>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFormLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QSpacerItem>
#include <QLabel>
#include <QMap>
#include <QString>
#include <QVariant>

#include "Controller.h"

class SignupData {
	public:
	enum FailureReason {
		EMAIL_ADDRESS_TAKEN,
		EMAIL_ADDRESS_MALFORMED,
		USERNAME_TAKEN,
		USERNAME_MALFORMED,
	};
		QString mEmailAddress;
		QString mFirstName;
		QString mLastName;
		QString mSex;
		QString mDesiredUsername;
		QString mPassword;
};

class SignupDataWidget: public QWidget {//LinearFormWidget {
	Q_OBJECT
	public:
		SignupDataWidget(QWidget* parent=NULL);
		QMap<QString, QVariant>* data() {return NULL;}
	signals:
		void attemptSignup(const QString& emailAddress, const QString& firstName, const QString& lastName, const QString& sex, const QString& desiredUsername, const QString& password);
	protected slots:
		void submit();
	protected:
		void initialize();

		QFormLayout mFormLayout;

		QLineEdit mEmailAddressLineEdit;

		QLineEdit mFirstNameLineEdit;
		QLineEdit mLastNameLineEdit;
		QComboBox mSexComboBox;
		
		QLineEdit mUsernameLineEdit;
		QLineEdit mPasswordLineEdit;
		QLineEdit mPasswordCheck;

		QHBoxLayout mButtonLayout;
		QPushButton mSubmitButton;
};

#endif
