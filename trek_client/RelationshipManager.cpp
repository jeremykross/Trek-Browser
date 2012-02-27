#include "RelationshipManager.h"

RelationshipManager::RelationshipManager() {
	connect(Controller::self(), SIGNAL(followedModelRequested(const QUuid&)), this, SLOT(controller_modelRequested(const QUuid&)));
	connect(Controller::self(), SIGNAL(fansModelRequested(const QUuid&)), this, SLOT(controller_modelRequested(const QUuid&)));
}	
RelationshipManager::~RelationshipManager() {
	QMap<QUuid, RelationshipData*>::iterator i = mRelationships.begin();
	while(i != mRelationships.end()) {
		delete i.value();
		i++;	
	}
}

void RelationshipManager::controller_modelRequested(const QUuid& guid) {
	if(mRelationships.contains(guid)) return;
	
	UserData* owner = UserDataFactory::self()->userDataForGuid(guid);	
	RelationshipData* relationship = new RelationshipData(owner);
	mRelationships.insert(guid, relationship);

}
