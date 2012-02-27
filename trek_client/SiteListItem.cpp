#include "SiteListItem.h"

SiteListItem::SiteListItem(SiteListItem*) {
	mParent=NULL;
}

SiteListItem* SiteListItem::parent() {
	return mParent;
}
QList<SiteListItem*>* SiteListItem::children() {
	return &mChildren; 
}

void SiteListItem::appendFlatDataList(QList<SiteData*> mData) {	
	for(int i=mData.count()-1; i>=0; i--) {
		SiteListItem* newChild = new SiteListItem(this);	
		newChild->mName = mData[i]->mName;
		newChild->mUrl = mData[i]->mUrl;
		addChild(newChild);
	}
}

void SiteListItem::addChild(SiteListItem* childItem) {
	mChildren.append(childItem);
	childItem->mParent = this;
	emit itemChanged(childItem, CHANGE_PARENT);
}
void SiteListItem::insertChild(int index, SiteListItem* childItem) {
	if(index > mChildren.count()) return;
	QMutableListIterator<SiteListItem*> it(mChildren);
	for(int i=0; i<index; i++) it.next();
	it.insert(childItem);
	childItem->mParent = this;
	emit itemChanged(childItem, CHANGE_PARENT);
	emit itemChanged(childItem, CHANGE_INDEX);
}
void SiteListItem::removeChild(SiteListItem* childItem) {
	mChildren.removeAll(childItem);
	childItem->mParent = NULL;
	emit itemChanged(childItem, CHANGE_PARENT);
}
void SiteListItem::removeChild(int index) {
	mChildren.removeAt(index);
}

SiteListItem* SiteListItem::takeChild(int index) {
	if(index > mChildren.count()) return NULL;
	QMutableListIterator<SiteListItem*> it(mChildren);
	for(int i=0; i<index; i++) it.next();
	SiteListItem* item = it.next();
	it.remove();
	item->mParent = NULL;
	emit itemChanged(item, CHANGE_PARENT);
	return item;
}


SiteListItem* SiteListItem::child(int index) {
	if(index > mChildren.count()) return NULL;
	QMutableListIterator<SiteListItem*> it(mChildren);
	for(int i=0; i<index; i++) it.next(); 
	return it.next();
}

int SiteListItem::childCount() {
	return mChildren.count();
}

int SiteListItem::indexOfChild(SiteListItem* child) {
	QListIterator<SiteListItem*> i(mChildren);
	int counter = 0;
	while(i.hasNext()) {
		if(child == i.next()) return counter;
		counter++; 
	}
	return -1;
}

void SiteListItem::debugPrint() {
	std::cout << mName.toStdString() << "\n";
	QListIterator<SiteListItem*> i(mChildren);
	while(i.hasNext()) {
		SiteListItem* curr = i.next();
		std::cout << curr->mName.toStdString() << "\n";	
	}
}


void SiteListItem::setName(const QString& name) {
	mName = name;
	emit itemChanged(this, CHANGE_NAME);
}
void SiteListItem::setUrl(const QString& url) {
	setUrl(QUrl(url));
}
void SiteListItem::setUrl(const QUrl& url) {
	mUrl = url;
	emit itemChanged(this, CHANGE_URL);
}
QString SiteListItem::name() {
	return mName;
}
QUrl SiteListItem::url() {
	return mUrl;
}


SiteListItem* SiteListItem::helper_getChildWith(SiteListItem* item, const QString& name, const QUrl& url) {
	for(int i=0; i<item->mChildren.count(); i++) {
		SiteListItem* toCheck = item->mChildren[i];
		SiteListItem* checkChildren = helper_getChildWith(toCheck, name, url);
		if(checkChildren) return checkChildren;
		if(name.isEmpty()==false) {
			if(toCheck->mName.compare(name)!=0) {
				//The names don't match
				continue;			
			}
		}
		if(url.isValid()) {
			if(toCheck->mUrl != url) {
				continue;
			}
		}
		return toCheck;
	}

	return NULL;

}

SiteListItem* SiteListItem::getChildWith(const QString& name, const QUrl& url) {
	return helper_getChildWith(this, name, url);
}
