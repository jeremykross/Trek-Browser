#ifndef USERSELECTIONSWIDGET_H
#define USERSELECTIONSWIDGET_H

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>

#include <QPalette>

#include "UserData.h"

class UserSelectionsWidget: public QWidget {
	Q_OBJECT
	public:	
		UserSelectionsWidget(UserData* d=new UserData(), QWidget* parent=NULL);
	signals:
		void privateChat(UserData* d);
		void profile(UserData* d);
		void toggleLiked(UserData* d);
	protected slots:
		void selectionMade();
	protected:	
		void initialize();
		UserData* mUserData;
		QPushButton mChatButton;
		QPushButton mProfileButton;
		QPushButton mLikeButton;
		QHBoxLayout* mHMainLayout;
};

#endif
