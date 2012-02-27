#ifndef PROFILEWIDGET_H
#define PROFILEWIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QDate>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCalendarWidget>
#include <QPalette>

#include "Controller.h"
#include "UserData.h"

class ProfileWidget: public QWidget {
	Q_OBJECT
	public:
		ProfileWidget(QWidget* parent=NULL);
		void setUserData(UserData* data);
		void setEditable(bool editable);
	signals:
		void controller_serverRequestProfile(const QUuid& guid);
		void controller_serverUpdateProfile(const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo);
		void canceled();
	protected slots:
		void commit();
		void controller_profileUpdated(const QUuid& guid, const QString& emailAddy, const QString& sex, const QDate& birthDate, const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo);
		void enableApply();
	protected:
		void initialize();
		UserData* mUserData;
		QGridLayout mGMainLayout;
		QFormLayout mFLayout;
		QDate mBirthDate;
		QLineEdit mUsernameLine;
		QLineEdit mEmailAddressLine;
		QLineEdit mBirthDateLine;
		QLineEdit mGenderLine;

		QTextEdit mOccupationText;
		QTextEdit mHobbiesText;
		QTextEdit mFavoriteCultureText;
		QTextEdit mAboutMeText;

		//QCalendarWidget mCalendarWidget;
		QPushButton mApplyButton;
		QPushButton mCancelButton;

		bool mEditable;
};

#endif
