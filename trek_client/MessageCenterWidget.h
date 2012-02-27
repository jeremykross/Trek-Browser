#include <QString>

#include <QTabWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QColor>
#include <QBrush>
#include <QTime>

#include "UserData.h"

#include <iostream>

#define CURRENT_TIME QTime::currentTime().toString("(hh:mm:ss) ")

class MessageCenterWidget:public QWidget {
	Q_OBJECT
	public:
		MessageCenterWidget(QWidget* parent=NULL);
		void logPersonalChat(UserData* data);
		void logExpeditionRequest(UserData* data);
		void logExpeditionRequestResponse(UserData* data, bool accepted);
		void logRequestAccept(UserData* data);
		void logRequestReject(UserData* data);
		void showChats();
		void showExpeditionRequests();
	//	void logFeed(UserData* data, const QString& mesg);
	signals:
		void personalChatRequested(UserData* data);
		void expeditionRequested(UserData* data);
	protected slots:
		void chatDoubleClicked(QListWidgetItem* item);
		void expeditionDoubleClicked(QListWidgetItem* item);
	protected:
		void initialize();
		QVBoxLayout mVMainLayout;
		QTabWidget mTabWidget;	
		QListWidget mFeedList;
		QListWidget mChatsList;
		QListWidget mExpeditionList;
};
