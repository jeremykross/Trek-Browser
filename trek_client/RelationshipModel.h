#ifndef RELATIONSHIPMODEL_H
#define RELATIONSHIPMODEL_H

/* Basic */
#include <QObject>
#include <QString>
#include <QBrush>
#include <QVariant>

/* Item Model */
#include <QAbstractItemModel>
#include <QAbstractListModel>

/* std */
#include <iostream>

#include "UserData.h"
#include "UserDataFactory.h"

class RelationshipListModel:public QAbstractListModel {
	Q_OBJECT
	public:
		RelationshipListModel();
		void setRelationshipList(QList<UserData*>* list);
		QList<UserData*>* relationshipList();
		int rowCount(const QModelIndex& index=QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role) const;
		bool insertRows(int row, int count, const QModelIndex& parent=QModelIndex());
		bool removeRows(int row, int count, const QModelIndex& parent=QModelIndex());
		UserData* userDataFromIndex(const QModelIndex& index);

		bool contains(UserData* data);
		void removeUsers(QList<UserData*> data);
		void removeUser(UserData* data);
		void addUsers(QList<UserData*> data);
		void addUser(UserData* data);
	protected slots:
		void dataAltered();
		void statusAltered();
	protected:
		QList<UserData*>* mRelationshipList;
};

/*class RelationshipModel:public QAbstractItemModel {
	public:
		RelationshipModel(QObject* parent=NULL);
		~RelationshipModel() {}
};*/


#endif
