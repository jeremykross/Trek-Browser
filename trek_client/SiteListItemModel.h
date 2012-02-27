#ifndef SITELISTITEMMODEL_H
#define SITELISTITEMMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QString>
#include <QModelIndex>
#include <QWebSettings>
#include <QFlags>
#include "SiteListItem.h"

/* Abandon hope all ye who enter. */

class SiteListItemModel:public QAbstractItemModel {
	public:
		SiteListItemModel(QObject* parent=NULL);
		~SiteListItemModel();

		void setRootItem(SiteListItem* item);
		void setEditable(bool on=true);
		bool isEditable();

		bool containsUrl(const QUrl& url);
			
		QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
		QModelIndex parent(const QModelIndex& child) const;
	
		int rowCount(const QModelIndex& parent) const;
		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& index, int role) const;
		bool setData(const QModelIndex& index, const QVariant& value, int role);
		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		Qt::ItemFlags flags(const QModelIndex& index) const;
		Qt::DropActions supportedDragActions() const;
		Qt::DropActions supportedDropActions() const;

		bool insertRows(int row, int count, const QModelIndex& index=QModelIndex());
		bool removeRows(int row, int count, const QModelIndex& index=QModelIndex());

		QModelIndex indexFromNode(SiteListItem* item);
		SiteListItem* nodeFromIndex(const QModelIndex& index) const;
		void addSite(const QString& name, const QUrl& url);

		void removeSite(const QString& name, const QUrl& url);
	protected:
		SiteListItem* mRoot;
		bool mEditable;
};

#endif
