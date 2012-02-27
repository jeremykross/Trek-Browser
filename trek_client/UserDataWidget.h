#ifndef USERDATAWIDGET_H
#define USERDATAWIDGET_H

#include <QWidget>
#include <QObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPoint>
#include <QContextMenuEvent>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QScrollArea>
#include <QUuid>

#include "UserData.h"
#include "UserInfoWidget.h"
#include "RelationshipWidget.h" 
#include "ProfileWidget.h"
#include "Controller.h"
#include "SiteListWidget.h"
#include "BookmarkWidget.h"
#include "HistoryWidget.h"

#define HIDESHOWTIME 250 
#define MINIMUMHEIGHT  150 //mUserInfoWidget->height() + 50  
#define XOFFSET 20

class UserDataWidget: public QWidget {
	Q_OBJECT
	public:	
		UserDataWidget(QWidget* parent=NULL);
		~UserDataWidget() {}
		void setUserData(UserData* data);
		UserData* userData();
		void showInfoWidget();
		void hideInfoWidget();
		QWidget* infoWidget();
	signals:
		void controller_serverRequestStatus(const QUuid& guid);
		void controller_userContextMenuRequested(UserData*);
		void closed(UserDataWidget* w);
	public slots:
		void userStatusAltered();
		void showFollowed(); 
		void showFans(); 
		void showProfile(); 
		void showFavorites();
		void showHistory();
	protected slots:
		void visibilityChanged(qreal value);
		void visibilityFinished();
		void setupFollowed(); 
		void setupFans(); 
		void setupProfile(); 
		void setupFavorites();
		void setupHistory();
	protected:
		void initialize();
		void setupActions();
		void clearInfo();
		void setInfo(const QString& labelText, QWidget* w);
		void paintEvent(QPaintEvent* evt);
		void resizeEvent(QResizeEvent* evt);
		void mousePressEvent(QMouseEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);
		void mouseMoveEvent(QMouseEvent* evt);
		void contextMenuEvent(QContextMenuEvent* evt);
		void closeEvent(QCloseEvent* evt);
		UserData* mData;
		UserInfoWidget* mUserInfoWidget;
		QWidget* mInfoWidget;
		QLabel l;
		QLabel mLocaleLabel;

		QVBoxLayout mVMainLayout;
		QTimeLine* mInfoVisibilityTimeLine;

		QAction* mShowProfileAction;
		QAction* mShowFollowedAction;
		QAction* mShowFansAction;
		QAction* mShowFriendsAction;


		int mOldX, mOldY;
		int mSavedHeight;
		int mSavedInfoHeight;
		bool mResizing;
		bool mShowing;
};	

#endif
