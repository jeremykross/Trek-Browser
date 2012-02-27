#ifndef SITELISTMANAGER_H
#define SITELISTMANAGER_H

/* Misc */
#include <QObject>
#include <QString>

#include "SiteListData.h"
#include "UserData.h"
#include "UserDataFactory.h"

/* So here's an interesting sub-system.  The SiteListManager catalogues site lists (i.e. favortes and history).  Because there are many instances of site-lists for various users, the SiteListManager makes server requests and stores the returned SiteListData for various users. */

class SiteListManager:public QObject {
	Q_OBJECT
	public:
		SiteListManager();
		~SiteListManager();
	protected slots:
		void controller_historyModelRequested(const QUuid& guid); 	
		void controller_favoritesModelRequested(const QUuid& guid);
	protected:
		QMap<QUuid, SiteListData*> mHistoryLists;
		QMap<QUuid, SiteListData*> mFavoriteLists;
};

#endif
