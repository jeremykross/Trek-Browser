#include "SiteListItemModel.h"

SiteListItemModel::SiteListItemModel(QObject*) {
	mRoot = NULL;	
	mEditable = false;
}
SiteListItemModel::~SiteListItemModel() {}

void SiteListItemModel::setEditable(bool on) {
	mEditable = on;
}
bool SiteListItemModel::isEditable() {
	return mEditable;
}
bool SiteListItemModel::containsUrl(const QUrl& url) {
	return (mRoot->getChildWith(QString(), url));
}

void SiteListItemModel::setRootItem(SiteListItem* item) {
	if(mRoot) delete mRoot;
	mRoot = item;
	reset();
}

QModelIndex SiteListItemModel::index(int row, int column, const QModelIndex& parent) const {
	if(mRoot == NULL || row < 0 || column < 0) return QModelIndex();

	SiteListItem* parentItem = nodeFromIndex(parent);
	//column and row are both relative to the parent
	if(parentItem) {
		SiteListItem* child = parentItem->child(row);	
		if(child) {
			return createIndex(row, column, child);
		} 
	}
	return QModelIndex();

}

QModelIndex SiteListItemModel::parent(const QModelIndex& child) const {
	SiteListItem* childItem = nodeFromIndex(child);
	if(childItem) {
		SiteListItem* parent = childItem->parent();
		if(parent) {
			SiteListItem* grandParent = parent->parent();
			if(grandParent) {
				int row = grandParent->children()->indexOf(parent);
				return createIndex(row, 0, parent);	
			}	
		}
	}
	return QModelIndex();
}

int SiteListItemModel::rowCount(const QModelIndex& parent) const {
	if(parent.column() > 0) return 0;

	SiteListItem* parentItem = nodeFromIndex(parent);
	if(parentItem) {
		return parentItem->children()->count();
	}
	return 0;
}
int SiteListItemModel::columnCount(const QModelIndex&) const {
	return 2;
}

QVariant SiteListItemModel::data(const QModelIndex& index, int role) const {
	if(index.isValid()) {
		SiteListItem* item = nodeFromIndex(index);
		if(role == Qt::DisplayRole) {
			if(index.column() == 0) {
				return item->name();
			} else if(index.column() == 1) {
				return item->url().toString();
			}
		} else if(role == Qt::ToolTipRole) {
			return item->name();
		} else if(role == Qt::DecorationRole) {
			if(index.column() == 0)
				return QWebSettings::iconForUrl(item->url());	
		}
	}
	return QVariant();
}

bool SiteListItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	if(index.isValid()) {
		SiteListItem* item = nodeFromIndex(index);
		if(item) {
			if(role == Qt::DisplayRole || role == Qt::EditRole) {
				if(index.column() == 0) {
					item->setName(value.toString());
					emit dataChanged(index, index);
					return true;
				} else if(index.column() == 1) {
					item->setUrl(value.toString());
					emit dataChanged(index, index);
					return true;
				}
			}
		}
	}
	return false;
}

QVariant SiteListItemModel::headerData(int section, Qt::Orientation orientation, int role) const  {
	if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
	if(section == 0) {
			return "Name";	
		} else if(section == 1) {
			return "Url";
		}
	}
	return QVariant();
}

Qt::ItemFlags SiteListItemModel::flags(const QModelIndex& index) const {
	Qt::ItemFlags flags; // = QAbstractItemModel::flags(index);

	flags |= Qt::ItemIsEnabled|Qt::ItemIsSelectable;
	if(index.parent().isValid() && mEditable) {
		//flags |= (Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled);
		if(index.column() == 0) {
			flags |= Qt::ItemIsEditable;
		}
	}
	return flags;
}

Qt::DropActions SiteListItemModel::supportedDragActions() const {
	return Qt::MoveAction;
}

Qt::DropActions SiteListItemModel::supportedDropActions() const {
	return Qt::MoveAction;
}

QModelIndex SiteListItemModel::indexFromNode(SiteListItem* item) {
	SiteListItem* parent = item->parent();
	if(parent) {
		int row = -1;
		for(int i=0; i<parent->childCount(); i++) {
			if(parent->child(i) == item) {
				row = i; 
				break;
			}
		}
		if(row != -1)
			return createIndex(row, 0, item);
	}
	return QModelIndex();
}

SiteListItem* SiteListItemModel::nodeFromIndex(const QModelIndex& index) const{
	if(index.isValid()) {
		return static_cast<SiteListItem*>(index.internalPointer());
	} else {
		return mRoot;
	}
}

void SiteListItemModel::removeSite(const QString& name, const QUrl& url) {
	SiteListItem* i = mRoot->getChildWith(name, url);
	if(i == NULL) return;
	removeRows(i->parent()->indexOfChild(i), 1, indexFromNode(i->parent()));
}
void SiteListItemModel::addSite(const QString& name, const QUrl& url) {
	SiteListItem* newChild = new SiteListItem();
	newChild->setName(name);
	newChild->setUrl(url);
	beginInsertRows(QModelIndex(), 0,0);
	mRoot->insertChild(0,newChild);
	endInsertRows();

/*	QModelIndex rootIndex = QModelIndex();
	mModel->insertRow(0, rootIndex);
	QModelIndex newName = mModel->index(0,0,rootIndex);
	mModel->setData(newName, name, Qt::DisplayRole);
	QModelIndex newUrl = mModel->index(0,1,rootIndex);
	mModel->setData(newUrl, url.toString(), Qt::DisplayRole); */
}

bool SiteListItemModel::insertRows(int row, int count, const QModelIndex& index) {
	beginInsertRows(index, row, row+count-1);	
	SiteListItem* node = nodeFromIndex(index);
	if(node) {
		for(int i=0; i<count; i++) {
			node->insertChild(row+i);
		}
		endInsertRows();
		return true;
	}
	endInsertRows();
	return false;
}

bool SiteListItemModel::removeRows(int row, int count, const QModelIndex& index) {
	beginRemoveRows(index, row, row+count-1);
	SiteListItem* node = nodeFromIndex(index);
	if(node) {
		for(int i=0; i<count; i++) {
			node->removeChild(row+i);	
		}
		endRemoveRows();
		return true;
	}
	endRemoveRows();
	return false;
}

