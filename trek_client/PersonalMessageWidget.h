#ifndef PERSONALMESSAGEWIDGET_H
#define PERSONALMESSAGEWIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QEvent>
#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QFont>
#include <QTextEdit>
#include <QSplitter>
#include <QPushButton>
#include <QToolBar>
#include <QList>
#include <QKeyEvent>
#include <QScrollBar>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QTime>

#include <iostream>

#include "Controller.h"
#include "UserData.h"


class PersonalMessageWidget:public QWidget {
	Q_OBJECT
	public:
		PersonalMessageWidget(UserData* userData=NULL, QWidget* parent=NULL);
		void setLocalData(UserData* userData);
		void setRemoteData(UserData* userData);
		bool eventFilter(QObject* watched, QEvent* evt);
		void logMessage(UserData* userData, const QString& mesg);
	signals:
		void controller_serverPersonalSpeak(const QUuid&, const QString&);
	protected slots:
		void userDataAltered();
		void send(); 
		void save();
		void controller_personalCommuniqueReceived(const QUuid& guid, const QString& mesg);
	protected:
		void initialize();
		UserData* mLocalData;
		UserData* mUserData;

		QGridLayout mGMainLayout;
		QLabel mUserIconLabel;
		QLabel mUsernameLabel;
		QTextEdit mConversationText;
		QTextEdit mInputText;
		QPushButton mSendButton;	
		QToolBar mToolbar;

		QAction* mSaveAction;
		QAction* mClearAction;
		
};

#endif
