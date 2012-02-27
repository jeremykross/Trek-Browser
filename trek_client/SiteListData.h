#ifndef SITELISTDATA_H
#define SITELISTDATA_H


/* Core */
#include <QObject>
#include <QString>
#include <QUuid>

#include "UserData.h"
#include "SiteListItem.h"
#include "Controller.h"

/* The SiteListData class manages the list of sites for a users favorites and history. */
class SiteListData:public QObject {
	Q_OBJECT
	public:
		SiteListData(UserData* owner);	
		~SiteListData();
		void requestData();
	signals:
		//The naming scheme is violated here a bit. 
		void controller_serverRequestList(const QUuid& guid);
		void controller_modelReceived(const QUuid& guid, SiteListItemModel* model);
	protected slots:
		void controller_listDataReceived(const QUuid& guid, QList<SiteData*> data);
		void controller_modelRequested(const QUuid& guid);
	protected:
		UserData* mUser;
		SiteListItem mRootItem;
		SiteListItemModel* mModel;
};

class SiteListDataConnection {
	public:	
		static void setupHistory(SiteListData* data);
		static void setupFavorites(SiteListData* data); 
};

#endif
