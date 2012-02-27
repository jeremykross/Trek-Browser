#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

/* Basic */
#include <QObject>
#include <QList>

/* Io */
#include <QFile>

/* Network */
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkCookieJar>

#include "Controller.h"
#include "DownloadData.h"
#include "DownloadModel.h"

class DownloadManager : public QObject {
	Q_OBJECT
	public:
		DownloadManager();
	signals:
		void downloadStarted(DownloadData* download);
		void controller_downloadModelReceived(DownloadModel* model);
		
		void controller_showSystemTrayMessage(const QString& title, const QString& mesg);
	public slots:
		void controller_downloadModelRequested();
		void downloadRequested(const QNetworkRequest& request);
		void downloadFinished();
		void downloadError(QNetworkReply::NetworkError error);
		void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
		void downloadReadyRead();
		void removeDownload();
		void stopDownload();
	protected:
		QNetworkAccessManager mNetworkAccess;
		QMap<QNetworkReply*, QFile*> mActiveDownloads;
		QMap<DownloadData*, QNetworkReply*> mDownloads;
		DownloadModel mModel;
};

#endif
