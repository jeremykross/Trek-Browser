#include "DownloadModel.h"

DownloadModel::DownloadModel(QObject* parent):QAbstractTableModel(parent) {}

void DownloadModel::setDownloadList(QList<DownloadData*> downloads) {
	mDownloads = downloads;
}

QList<DownloadData*>* DownloadModel::downloads() {
	return &mDownloads;
}
void DownloadModel::addDownload(DownloadData* download) {
	beginInsertRows(QModelIndex(), mDownloads.count(), mDownloads.count());	
	mDownloads.append(download);
	endInsertRows();
	connect(download, SIGNAL(progress(qint64, qint64)), this, SLOT(updateData()));
	//connect(download, SIGNAL(stopMe()), this, SLOT(updateData()));
	connect(download, SIGNAL(done()), this, SLOT(updateData()));
}
void DownloadModel::removeDownload(DownloadData* download) {	
	int index = mDownloads.indexOf(download); 
	beginRemoveRows(QModelIndex(), index, index);	
        mDownloads.takeAt(index);
	endRemoveRows();
}
void DownloadModel::informOfChange(DownloadData* download) {
	updateData(download);	
}

int DownloadModel::rowCount(const QModelIndex&) const{
	return mDownloads.count();
}
int DownloadModel::columnCount(const QModelIndex&) const {
	return 2;
}

QVariant DownloadModel::data(const QModelIndex& index, int role) const {
	DownloadData* dl = mDownloads.value(index.row());
	if(role == Qt::DisplayRole && index.column() == 0) {
		return dl->localName();
	} else if(role == Qt::DisplayRole && index.column() == 1) {
		return QVariant(dl->percentageComplete());
	} else if(role == Qt::UserRole) {
		return dl->bytesReceived();
	} else if(role == Qt::UserRole+1) {
		return dl->bytesTotal();
	} else if(role == Qt::UserRole+2) {
		return dl->isStopped();
	} else if(role == Qt::UserRole+3) {
		return dl->isFinished();
	}
	return QVariant();
}

QVariant DownloadModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role != Qt::DisplayRole) return QVariant();
	if(orientation == Qt::Vertical) return section;
	if(section == 0) return "File Name";
	if(section == 1) return "Progress";
        return "Ermac";
}


void DownloadModel::updateData() {
	DownloadData* dlData = (DownloadData*)sender();
	updateData(dlData);
}
void DownloadModel::updateData(DownloadData* dlData) {
	int localIndex = mDownloads.indexOf(dlData);
	QModelIndex modelIndex = index(localIndex,1);
	emit dataChanged(modelIndex, modelIndex);
}


DownloadData* DownloadModel::dataFromIndex(const QModelIndex& index) {
	DownloadData* dl = mDownloads.value(index.row());
	return dl;
}

