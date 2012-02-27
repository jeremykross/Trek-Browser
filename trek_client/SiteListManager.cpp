#include "SiteListManager.h"

SiteListManager::SiteListManager() {
	connect(Controller::self(), SIGNAL(favoritesModelRequested(const QUuid&)), this, SLOT(controller_favoritesModelRequested(const QUuid&)));	
	connect(Controller::self(), SIGNAL(historyModelRequested(const QUuid&)), this, SLOT(controller_historyModelRequested(const QUuid&)));	
}
SiteListManager::~SiteListManager() {
	QMap<QUuid, SiteListData*>::iterator hi = mHistoryLists.begin();
	while(hi != mHistoryLists.end()) {
		delete hi.value();
		hi ++;
	}
	QMap<QUuid, SiteListData*>::iterator fi = mFavoriteLists.begin();
	while(fi != mFavoriteLists.end()) {
		delete fi.value();
		fi ++;
	}
}

void SiteListManager::controller_favoritesModelRequested(const QUuid& guid) {
	if(mFavoriteLists.contains(guid)) return;

	UserData* owner = UserDataFactory::self()->userDataForGuid(guid);
	SiteListData* data = new SiteListData(owner);
	SiteListDataConnection::setupFavorites(data);
	data->requestData();
	mFavoriteLists.insert(guid, data);
}
void SiteListManager::controller_historyModelRequested(const QUuid& guid) {
	if(mHistoryLists.contains(guid)) return;

	UserData* owner = UserDataFactory::self()->userDataForGuid(guid);
	SiteListData* data = new SiteListData(owner);
	SiteListDataConnection::setupHistory(data);
	data->requestData();
	mHistoryLists.insert(guid, data);
}
