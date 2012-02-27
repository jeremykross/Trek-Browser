#ifndef DOWNLOADDATA_H
#define DOWNLOADDATA_H

#include <QObject>
#include <QNetworkReply>
#include <QString>
#include <QFileInfo>
#include <QTimer>


class DownloadData: public QObject {
	Q_OBJECT
	public:	
		DownloadData();
		QString remotePath();
		QString localPath();
		QString localName();
		qint64 bytesReceived();
		qint64 bytesTotal();
		qreal percentageComplete();
		int transferRate() {return -1;}
		bool isFinished();
		bool isStopped();
		void stop();
		void remove();
	signals:
		void error(QNetworkReply::NetworkError error);
		void progress(qint64 bytesReceived, qint64 bytesSent);
		void done();
		void stopMe();
		void removeMe();
		//void transferRate(qint64 bytesPerSecond);
	protected slots:
		void catchProgress(qint64 bytesReceived, qint64 bytesTotal);
		void finished();
	protected:
		QString mRemotePath;
		QString mLocalPath;
		qint64 mBytesReceived;
		qint64 mBytesTotal;
		float mTransferRate;
		bool mIsFinished;
		bool mIsStopped;

	friend class DownloadManager;
};

#endif
