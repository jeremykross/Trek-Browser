#ifndef RELATIONSHIPWIDGET_H
#define RELATIONSHIPWIDGET_H

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QVBoxLayout>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QSortFilterProxyModel>

#include <iostream>

#include "Controller.h"
#include "RelationshipModel.h"
#include "UserData.h"

class RelationshipWidget:public QWidget {
	Q_OBJECT
	public:
		RelationshipWidget();
		UserData* currentSelection();
		QSortFilterProxyModel* proxyModel();
		void setUserData(UserData* data);
		void setViewFollowed();
		void setViewFans();
		void setViewFriends() {}
	signals:
		void controller_modelRequested(const QUuid&);
		void controller_userContextMenuRequested(UserData* data);
		void controller_userDoubleClicked(UserData* data);

		void doubleClicked(UserData* data);

	protected slots:
		void controller_modelOffered(const QUuid& guid, RelationshipListModel* model);
		void catchDoubleClick(const QModelIndex& index);	
	protected:
		void contextMenuEvent(QContextMenuEvent* evt); 
		UserData* mOwner;
		RelationshipListModel* mModel;
		QSortFilterProxyModel mProxyModel;
		QVBoxLayout mVMainLayout;
		QListView mView;

};

class CompoundRelationshipWidget: public QTabWidget {
	Q_OBJECT
	public:
		CompoundRelationshipWidget(QWidget* parent=NULL);
		void setOwner(UserData* owner);
		UserData* currentSelection();
		RelationshipWidget* followedWidget();
		RelationshipWidget* fansWidget();
	signals:
		void doubleClicked(UserData* data);
	protected:
		UserData* mOwner;
		RelationshipWidget mFollowedWidget;
		RelationshipWidget mFansWidget; 
		
};

#endif
