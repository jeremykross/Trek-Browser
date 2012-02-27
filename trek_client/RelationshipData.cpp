#include "RelationshipData.h"

RelationshipData::RelationshipData(UserData* owner) {
	mUser = owner;
	mFollowedModel = NULL;
	mFansModel = NULL;

	connect(this, SIGNAL(controller_serverRequestFollowed(const QUuid&)), Controller::self(), SIGNAL(serverRequestFollowed(const QUuid&)));
	connect(Controller::self(), SIGNAL(followedReceived(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverReceiveFollowed(const QUuid&, QList<UserData*>))); 
	//connect(this, SIGNAL(controller_serverRequestFriends(const QUuid&)), Controller::self(), SIGNAL(serverRequestFriends(const QUuid&)));
	//connect(Controller::self(), SIGNAL(friendsReceived(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverReceiveFriends(const QUuid&, QList<UserData*>)));
	connect(this, SIGNAL(controller_serverRequestFans(const QUuid&)), Controller::self(), SIGNAL(serverRequestFans(const QUuid&)));
	connect(Controller::self(), SIGNAL(fansReceived(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverReceiveFans(const QUuid&, QList<UserData*>)));

	connect(Controller::self(), SIGNAL(serverRemoveFans(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverRemoveFans(const QUuid&, QList<UserData*>)));
	connect(Controller::self(), SIGNAL(serverRemoveFollowed(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverRemoveFollowed(const QUuid&, QList<UserData*>)));

	connect(Controller::self(), SIGNAL(followedModelRequested(const QUuid&)), this, SLOT(controller_followedModelRequested(const QUuid&)));
	connect(this, SIGNAL(controller_followedModelOffered(const QUuid&, RelationshipListModel*)), Controller::self(), SIGNAL(followedModelOffered(const QUuid&, RelationshipListModel*)));

	connect(Controller::self(), SIGNAL(fansModelRequested(const QUuid&)), this, SLOT(controller_fansModelRequested(const QUuid&)));
	connect(this, SIGNAL(controller_fansModelOffered(const QUuid&, RelationshipListModel*)), Controller::self(), SIGNAL(fansModelOffered(const QUuid&, RelationshipListModel*)));

	emit controller_serverRequestFollowed(mUser->guid());
	emit controller_serverRequestFans(mUser->guid());
}

RelationshipData::~RelationshipData() {	
	if(mFollowedModel) {
		delete mFollowedModel;
	}	
	if(mFansModel) {
		delete mFansModel;
	}
}

void RelationshipData::controller_serverRemoveFollowed(const QUuid& owner, QList<UserData*> followed) {
	if(owner != mUser->guid()) return;
	for(int i=0; i<followed.count(); i++) {
		int index = mFollowed.indexOf(followed[i]);
		if(index != -1) {
			if(mFollowedModel) {
				mFollowedModel->removeRow(index);
			}
		}
	}
}
void RelationshipData::controller_serverRemoveFans(const QUuid& owner, QList<UserData*> fans) {
	if(owner != mUser->guid()) return;
	for(int i=0; i<fans.count(); i++) {
		int index = mFans.indexOf(fans[i]);
		if(index != -1) {
			if(mFansModel) {
				mFansModel->removeRow(index);
			}
		}
	}
}
void RelationshipData::controller_serverReceiveFans(const QUuid& owner, QList<UserData*> fans) {
	if(owner != mUser->guid()) return;
	//disconnect(Controller::self(), SIGNAL(fansReceived(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverReceiveFans(const QUuid&, QList<UserData*>))); 
	mFans.append(fans);

	//	std::cout << " PRINTING FANS: \n";
	//	for(int i=0; i<mFans.size(); i++) {
	//		std::cout << mFans[i]->displayName().toStdString() << "\n";
	//	}	
	if(mFansModel == NULL) {
		mFansModel = new RelationshipListModel();
		mFansModel->setRelationshipList(&mFans);

		emit controller_fansModelOffered(mUser->guid(), mFansModel);
	} else {
		mFansModel->insertRows(mFansModel->rowCount()-1, fans.count());	
	}

}

void RelationshipData::controller_serverReceiveFollowed(const QUuid& owner, QList<UserData*> followed) {
	if(owner != mUser->guid()) return;
	//disconnect(Controller::self(), SIGNAL(followedReceived(const QUuid&, QList<UserData*>)), this, SLOT(controller_serverReceiveFollowed(const QUuid&, QList<UserData*>))); 
	mFollowed.append(followed);
	if(mFollowedModel == NULL) {
		mFollowedModel = new RelationshipListModel();
		mFollowedModel->setRelationshipList(&mFollowed);
		emit controller_followedModelOffered(mUser->guid(), mFollowedModel);
	} else {
		mFollowedModel->insertRows(mFollowedModel->rowCount()-1, followed.count());
	}
}

void RelationshipData::controller_followedModelRequested(const QUuid& guid) {
	if(guid != mUser->guid() || mFollowedModel == NULL) return;

	emit controller_followedModelOffered(guid, mFollowedModel); 
}

void RelationshipData::controller_fansModelRequested(const QUuid& guid) {
	if(guid != mUser->guid() || mFansModel == NULL) return;
	emit controller_fansModelOffered(guid, mFansModel);
}

