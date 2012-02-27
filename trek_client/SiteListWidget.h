#ifndef SITELISTWIDGET_H
#define SITELISTWIDGET_H

#include <QWidget>
#include <QObject>
#include <QTreeView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPalette>
#include <QColor>
#include <QSortFilterProxyModel>

#include <iostream>

#include "SiteListItem.h"
#include "SiteListItemModel.h"
#include "Controller.h"


class SiteListWidget: public QWidget {
	Q_OBJECT
	public:
		SiteListWidget(UserData* userData=NULL, QWidget* parent=NULL);
		SiteListItemModel* model();
		QSortFilterProxyModel* proxyModel();
		QTreeView* view();
		void setOwner(UserData* userData);
		void requestData();
	signals:
		void requestSiteListModel(const QUuid& guid);
		void urlDoubleClicked(const QUrl& url);
	public slots:
		void receiveSiteListModel(const QUuid& guid, SiteListItemModel* model);
		void doubleClicked(const QModelIndex& index);
	protected:
		virtual void initialize();
		UserData* mUserData;
		QVBoxLayout mMainLayout;
		QTreeView mItemTree;
		SiteListItemModel* mModel;
		QSortFilterProxyModel mProxyModel;
};

class SiteListWidgetConnection {
	public:
		static void setupBookmark(SiteListWidget* widget);
		static void setupHistory(SiteListWidget* widget);
	private:
		SiteListWidgetConnection();
};
#endif
