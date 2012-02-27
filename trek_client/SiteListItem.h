#ifndef SITELISTITEM_H
#define SITELISTITEM_H

#include <QObject>
#include <QString>
#include <QUrl>
#include <QLinkedList>
#include <QMutableLinkedListIterator>
#include <QLinkedListIterator>
#include <QMutableListIterator>
#include <QListIterator>

#include <iostream>

/* Abandon hope all ye who enter. */
struct SiteData {
	QString mName;
	QUrl mUrl;
};

class SiteListItem:public QObject {
	Q_OBJECT
	public:
		enum Change {
			CHANGE_PARENT,
			CHANGE_INDEX,
			CHANGE_NAME,
			CHANGE_URL
		};
		SiteListItem(SiteListItem* parent=NULL);
		SiteListItem* parent();
		QList<SiteListItem*>* children();

		void appendFlatDataList(QList<SiteData*> mData);

		void addChild(SiteListItem* childItem);
		void insertChild(int index, SiteListItem* childItem=new SiteListItem());
		void removeChild(SiteListItem* childItem);
		void removeChild(int index);

		SiteListItem* takeChild(int index);
		int indexOfChild(SiteListItem* child);	
		SiteListItem* child(int index);
		int childCount();
		void setName(const QString& name);
		void setUrl(const QString& url);
		void setUrl(const QUrl& url);

		//QLinkedListIterator<SiteListItem*> iterator();
		//QMutableLinkedListIterator<SiteListItem*> mutableIterator();

		QString name();
		QUrl url();
		
		SiteListItem* getChildWith(const QString& name, const QUrl& url); 

		void debugPrint();
	signals:
		void itemChanged(SiteListItem* me, SiteListItem::Change typeOfChange);
	protected:
		SiteListItem* helper_getChildWith(SiteListItem* item, const QString& name, const QUrl& url);
		SiteListItem* mParent;
		QList<SiteListItem*> mChildren;
		QString mName;
		QUrl mUrl;

	friend class SiteListItemModel;
};

#endif
