#ifndef ADMINCOMMANDWIDGET_H
#define ADMINCOMMANDWIDGET_H

#include <QWidget>
#include <QObject>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QKeyEvent>

#include "Controller.h"

class AdminCommandWidget:public QWidget {
	Q_OBJECT
	public:
		AdminCommandWidget(QWidget* w=NULL);
	signals:
		void controller_serverExecuteAdminCommand(const QString& username, int command, const QString& argument);
	protected slots:
		void execute();
	protected:
		void initialize();
		QFormLayout* mFMainLayout;
		QLineEdit mUsernameLine;
		QLineEdit mOptionLine;
		QComboBox mCommandCombo;
		QPushButton mExecuteButton;

};

#endif
