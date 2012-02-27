#ifndef RELATIONSHIPDATA_H
#define RELATIONSHIPDATA_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList>
#include <QMessageBox>
#include <QtAlgorithms>

#include <iostream>

#include "Controller.h"
#include "UserData.h"
#include "RelationshipModel.h"

class RelationshipData: public QObject {
	Q_OBJECT
	public:
		RelationshipData(UserData* owner);
		~RelationshipData();
	signals:
		void controller_serverRequestFollowed(const QUuid& owner);
		//void controller_serverRequestFriends(const QUuid&);
		void controller_serverRequestFans(const QUuid&);

		void controller_followedModelOffered(const QUuid& owner, RelationshipListModel* model);
		void controller_fansModelOffered(const QUuid& owner, RelationshipListModel* model);
	protected slots:
		void controller_serverReceiveFollowed(const QUuid& owner, QList<UserData*> followed);
		//void controller_serverReceiveFriends(const QUuid& owner, QList<UserData*> followed) {}
		void controller_serverReceiveFans(const QUuid& owner, QList<UserData*> followed);
		void controller_serverRemoveFans(const QUuid& owner, QList<UserData*> fans);
		void controller_serverRemoveFollowed(const QUuid& owner, QList<UserData*> followed);
		void controller_followedModelRequested(const QUuid& guid);
		void controller_fansModelRequested(const QUuid& guid);
	protected:
		UserData* mUser;
		QList<UserData*> mFollowed;
		QList<UserData*> mFans;
		RelationshipListModel* mFollowedModel;
		RelationshipListModel* mFansModel;
};

#endif
