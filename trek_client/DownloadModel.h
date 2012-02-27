#ifndef DOWNLOADMODEL_H
#define DOWNLOADMODEL_H

#include <QObject>
#include <QVariant>
#include <QAbstractTableModel>
#include <QList>
#include <QString>

#include "DownloadData.h"

class DownloadModel:public QAbstractTableModel {
	Q_OBJECT
	public:
		DownloadModel(QObject* parent=NULL);
		void setDownloadList(QList<DownloadData*> downloads);
		int rowCount(const QModelIndex& parent) const;
		int columnCount(const QModelIndex& parent) const;
		QVariant data(const QModelIndex& index, int role) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role) const;
		void addDownload(DownloadData* download);
		void removeDownload(DownloadData* download);
		void informOfChange(DownloadData* download);
		DownloadData* dataFromIndex(const QModelIndex& index);
		QList<DownloadData*>* downloads();
	protected slots:
		void updateData();
		void updateData(DownloadData* download);
	protected:
		QList<DownloadData*> mDownloads;
};


#endif
