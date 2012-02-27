#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QColor>
#include <QObject>
#include <QTextEdit>
#include <QTextBrowser>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QScrollBar>
#include <QTextCursor>
#include <QTime>
#include <QSizePolicy>

#include "Controller.h"

//Leak Free
class ChatWidget : public QWidget {
	Q_OBJECT;
	
	public: 
		ChatWidget(QWidget* parent=0);
                ~ChatWidget();
		QTextBrowser* textEdit();
		QLineEdit* lineEdit();
		void setLogVisible(bool value);
		
	protected:
		void beginLog();
		void endLog();
		void initialize();
		QVBoxLayout* mainLayout;
		QHBoxLayout* inputLayout;
		QTextBrowser* chatBox;
		QLineEdit* inputBox;
	
	protected slots:
		void sendMessage();
		void truncateMessage();
		
	public slots: 
		void logEntrance(UserData* userData);
		void logExit(UserData* userData, const QUrl& newUrl=QUrl());
		void logMessage(UserData* userData, const QString& mesg);
		void reset(const QString& mRoomName="Chat");
		
	signals:
		void messageSent(const QString& mesg);		
			
};

#endif

