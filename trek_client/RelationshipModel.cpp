#include "RelationshipModel.h"

RelationshipListModel::RelationshipListModel() {
        mRelationshipList = NULL;
}

void RelationshipListModel::setRelationshipList(QList<UserData*>* list) {
	mRelationshipList = list;
	for(int i=0; i<mRelationshipList->count(); i++) {
		connect(mRelationshipList->value(i), SIGNAL(dataAltered()), this, SLOT(dataAltered()));
		connect(mRelationshipList->value(i), SIGNAL(statusAltered()), this, SLOT(statusAltered()));
	}
	reset();
}	
QList<UserData*>* RelationshipListModel::relationshipList() {
	return mRelationshipList;
}

void RelationshipListModel::dataAltered() {
//	std::cout << "Data was altered\n";
//	reset();		
}	
void RelationshipListModel::statusAltered() {
	reset();
}

int RelationshipListModel::rowCount(const QModelIndex&) const {
        if(mRelationshipList) {
                return  mRelationshipList->count();
        } else {
                return -1;
        }
}	

QVariant RelationshipListModel::data(const QModelIndex& index, int role) const {
	if(index.isValid()) {
		if(index.row() >= mRelationshipList->count()) {
			return QVariant();
		}			
	

		UserData* d = mRelationshipList->value(index.row());
		if(d == NULL) return QVariant();
//		if(d->hasIcon()==false)
//			UserDataFactory::self()->requestAppearance(d->guid());
		if(role == Qt::DisplayRole) {
			return d->displayName()+"\n"+d->statusAsString();
		} else if(role == Qt::DecorationRole) {
			return d->scaledIcon();	
		} else if(role == Qt::BackgroundRole) {
			return QBrush(d->usernameBGColor());
		} else if(role == Qt::ForegroundRole) {
			return QBrush(d->usernameFontColor());
		}
	}
	return QVariant();
}

bool RelationshipListModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row+count-1); 
	endInsertRows();
	return true;
}
bool RelationshipListModel::removeRows(int row, int count, const QModelIndex&) {
	beginRemoveRows(QModelIndex(), row, row+count-1);	
	for(int i=0; i<count; i++)
		mRelationshipList->removeAt(row+i);		
	endRemoveRows();
	return true;
}

UserData* RelationshipListModel::userDataFromIndex(const QModelIndex& index) {
	if(index.isValid()) {
		return mRelationshipList->value(index.row()); 
	} else {
		return NULL;
	}
}	

bool RelationshipListModel::contains(UserData* data) {
	return mRelationshipList->contains(data); 
}
void RelationshipListModel::removeUsers(QList<UserData*> data) {
	for(int i=0; i<data.count(); i++) {
		removeUser(data[i]);
	}
}
void RelationshipListModel::removeUser(UserData* data) {
	int indexOf = mRelationshipList->indexOf(data);
	if(indexOf != -1) {
		removeRow(indexOf);	
	}
}
void RelationshipListModel::addUsers(QList<UserData*> data) {
	for(int i=0; i<data.count(); i++) {
		addUser(data[i]);
	}
}
void RelationshipListModel::addUser(UserData* data) {
	mRelationshipList->append(data);
	connect(data, SIGNAL(dataAltered()), this, SLOT(dataAltered()));
	connect(data, SIGNAL(statusAltered()), this, SLOT(statusAltered()));
	insertRow(mRelationshipList->count()-1);
}



