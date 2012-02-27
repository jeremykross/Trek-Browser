#ifndef PASSWORDCHANGEWIDGET_H
#define PASSWORDCHANGEWIDGET_H
#include <QWidget>
#include <QObject>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>

#include "Controller.h"

class PasswordChangeWidget: public QWidget {
	Q_OBJECT
	public:
		PasswordChangeWidget(QWidget* w=NULL);
	signals:	
		void controller_serverChangePassword(const QString& newPass);
	protected slots:
		void change();
	protected:
		void initialize();
		QVBoxLayout* mVMainLayout;
		QFormLayout* mFLayout;
		QLabel mLabel;
		QLineEdit mPasswordEdit;
		QLineEdit mPWCheckEdit;
		QPushButton mChangeButton;
};

#endif

