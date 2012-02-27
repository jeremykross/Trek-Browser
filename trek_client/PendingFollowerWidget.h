#ifndef PENDINGFOLLOWERWIDGET_H
#define PENDINGFOLLOWERWIDGET_H

#include <QWidget>
#include <QObject>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QStringList>
#include <QIcon>
#include <QSize>
#include <QBrush>
#include <QVariant>
#include <QMessageBox>
#include <QCursor>
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QApplication>

#include "UserData.h"
#include "Controller.h"

class PendingFollowerWidget: public QWidget {
	Q_OBJECT
	public:
		PendingFollowerWidget(QWidget* parent=NULL);
		void setOwner(UserData* userData);
	signals:
		void controller_serverAllowFollower(const QUuid& guid);
		void controller_serverDenyFollower(const QUuid& guid);
		void controller_serverRevokeFollowRequest(const QUuid& guid);
		void controller_showSystemTrayMessage(const QString& title, const QString& message);
		void controller_userContextMenuRequested(UserData*);
	protected slots:
		void controller_pendingFollowersReceived(QList<UserData*> followers);
		void controller_pendingFollowingReceived(QList<UserData*> following);
		void controller_followedReceived(const QUuid& ownder, QList<UserData*> followed);
		void itemDoubleClicked(QTreeWidgetItem* i, int col);
		void itemClicked(QTreeWidgetItem* i, int col);
		void itemPressed(QTreeWidgetItem* i, int col);
		void allow(QTreeWidgetItem* i);
		void deny(QTreeWidgetItem* i);
		void checkAll();
		void uncheckAll();
		void allowChecked();
		void denyChecked();

		void helperUpdatePixmaps();
	protected:
		void initialize();
		void showButtonsIfNeeded();
		UserData* mOwner;
		QTreeWidget mTreeWidget;		
		QTreeWidgetItem mOutgoingTree;
		QTreeWidgetItem mIncomingTree;
		QPushButton mYesButton;
		QPushButton mNoButton;
		QHBoxLayout* mHButtonLayout;
		QVBoxLayout* mVMainLayout;

		bool mLeftDown;
};

#endif
