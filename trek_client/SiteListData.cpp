#include "SiteListData.h"


SiteListData::SiteListData(UserData* owner) {
	mModel = NULL;
	mUser = owner;
}
SiteListData::~SiteListData() {
	if(mModel) {
		delete mModel;
		mModel = NULL;
	}
}

void SiteListData::requestData() {
	if(mUser)
		emit controller_serverRequestList(mUser->guid());
}

void SiteListData::controller_listDataReceived(const QUuid& guid, QList<SiteData*>data) {
	std::cout << "LIST DATA RECEIEVED\n";
	if(guid != mUser->guid()) return;
	if(mModel == NULL) {
		mModel =new SiteListItemModel();
		mModel->setRootItem(&mRootItem);
	}
	mRootItem.children()->clear();
	mRootItem.appendFlatDataList(data);
	emit controller_modelReceived(mUser->guid(), mModel);

	//FREE ME!
	for(int i=0; i<data.count(); i++) {
		if(data[i]) delete data[i];
		data[i] = NULL;
	}

//	disconnect(Controller::self(), SIGNAL(favoritesDataReceived(const QUuid&, QList<SiteData*>)), this, SLOT(controller_listDataReceived(const QUuid&, QList<SiteData*>)));
}
void SiteListData::controller_modelRequested(const QUuid& guid) {
	if(guid != mUser->guid() || mModel==NULL) return;
	requestData();
	//emit controller_modelReceived(guid, mModel);
}


void SiteListDataConnection::setupHistory(SiteListData* data) {
	QObject::connect(data, SIGNAL(controller_serverRequestList(const QUuid&)), Controller::self(), SIGNAL(serverRequestHistory(const QUuid&)));	
	QObject::connect(Controller::self(), SIGNAL(historyDataReceived(const QUuid&, QList<SiteData*>)), data, SLOT(controller_listDataReceived(const QUuid&, QList<SiteData*>)));
	QObject::connect(Controller::self(), SIGNAL(historyModelRequested(const QUuid&)), data, SLOT(controller_modelRequested(const QUuid&)));
	QObject::connect(data, SIGNAL(controller_modelReceived(const QUuid&, SiteListItemModel*)), Controller::self(), SIGNAL(historyModelReceived(const QUuid&, SiteListItemModel*)));
}
void SiteListDataConnection::setupFavorites(SiteListData* data) {
	QObject::connect(data, SIGNAL(controller_serverRequestList(const QUuid&)), Controller::self(), SIGNAL(serverRequestFavorites(const QUuid&)));	
	QObject::connect(Controller::self(), SIGNAL(favoritesDataReceived(const QUuid&, QList<SiteData*>)), data, SLOT(controller_listDataReceived(const QUuid&, QList<SiteData*>)));
	QObject::connect(Controller::self(), SIGNAL(favoritesModelRequested(const QUuid&)), data, SLOT(controller_modelRequested(const QUuid&)));
	QObject::connect(data, SIGNAL(controller_modelReceived(const QUuid&, SiteListItemModel*)), Controller::self(), SIGNAL(favoritesModelReceived(const QUuid&, SiteListItemModel*)));
}
