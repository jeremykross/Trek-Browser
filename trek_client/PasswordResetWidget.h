#ifndef PASSWORDRESETWIDGET_H
#define PASSWORDRESETWIDGET_H
#include <QWidget>
#include <QObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFont>
#include <QKeyEvent>
#include <QKeySequence>

#include "Controller.h"

class PasswordResetWidget:public QWidget {
	Q_OBJECT
	public:
		PasswordResetWidget(QWidget* parent=NULL);
	signals:
		void controller_serverRequestPasswordReset();
		void canceled();
	protected:
		void keyPressEvent(QKeyEvent* evt);
		void initialize();
		QLabel mLabel;
		QPushButton mResetButton;
		QPushButton mCancelButton;
		QVBoxLayout* mVMainLayout;
		QHBoxLayout* mHButtonLayout;
};

#endif
