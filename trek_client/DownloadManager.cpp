#include "DownloadManager.h"

DownloadManager::DownloadManager() {
	connect(Controller::self(), SIGNAL(downloadModelRequested()), this, SLOT(controller_downloadModelRequested()));
	connect(this, SIGNAL(controller_downloadModelReceived(DownloadModel*)), Controller::self(), SIGNAL(downloadModelReceived(DownloadModel*)));
	connect(Controller::self(), SIGNAL(downloadRequested(const QNetworkRequest&)), this, SLOT(downloadRequested(const QNetworkRequest&)));	
	connect(this, SIGNAL(downloadStarted(DownloadData*)), Controller::self(), SIGNAL(downloadStarted(DownloadData*)));

	connect(this, SIGNAL(controller_showSystemTrayMessage(const QString&, const QString&)), Controller::self(), SIGNAL(showSystemTrayMessage(const QString&, const QString&)));
}

void DownloadManager::controller_downloadModelRequested() {
	emit controller_downloadModelReceived(&mModel);
}

void DownloadManager::downloadRequested(const QNetworkRequest& request) {
	std::cout << "DownloadManager::DownloadRequested()\n";
	QString src = request.url().toString();
	QString dest = request.attribute(QNetworkRequest::User, "~").toString();

	QFile* fileDestination = new QFile(dest);
	fileDestination->open(QIODevice::ReadWrite);
	QNetworkReply* reply = mNetworkAccess.get(request);
	reply->setReadBufferSize(0);
	mActiveDownloads.insert(reply, fileDestination);

	connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));

	DownloadData* data = new DownloadData();
	data->mRemotePath = src;
	data->mLocalPath = dest;

	connect(reply, SIGNAL(finished()), data, SLOT(finished()));
	
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), data, SLOT(catchProgress(qint64, qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), data, SIGNAL(error(QNetworkReply::NetworkError)));
	connect(data, SIGNAL(stopMe()), this, SLOT(stopDownload()));
	connect(data, SIGNAL(removeMe()), this, SLOT(removeDownload()));
	//connect(data, SIGNAL(reloadMe()), this, SLOT(reloadDownload()));
	
	emit downloadStarted(data);
	emit controller_showSystemTrayMessage("", "Download Started: " + data->localName());
	mModel.addDownload(data);
	mDownloads.insert(data, reply);
}

void DownloadManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
	std::cout << "Progress: " << bytesReceived << " bytes of " << bytesTotal << "\n";	
}

void DownloadManager::downloadFinished() {
	QNetworkReply* reply = (QNetworkReply*)sender(); 
	QFile* fileDest = mActiveDownloads[reply]; 
	fileDest->write(reply->readAll());
	fileDest->close();

	emit controller_showSystemTrayMessage("", "Download Finished: " + fileDest->fileName()); 
}

void DownloadManager::downloadError(QNetworkReply::NetworkError error) {
	QMessageBox::critical(NULL, "Download Error!", QString("Error code: %1").arg(error));
}

void DownloadManager::downloadReadyRead() {
	//QNetworkReply* reply = (QNetworkReply*)sender();
	//QFile* fileDest = mActiveDownloads[reply];
	//fileDest->write(reply->read(reply->bytesAvailable()));	
}

void DownloadManager::stopDownload() {
	DownloadData* d = ((DownloadData*)sender());
	QNetworkReply* reply = mDownloads[d];	
	reply->abort();
	d->mIsFinished = true;
	d->mIsStopped = true;
	d->mBytesReceived = -1;
	mModel.informOfChange(d);
//	QFile* fileDest = mActiveDownloads[reply]; 
//	fileDest->close();
}

void DownloadManager::removeDownload() {
	DownloadData* d = ((DownloadData*)sender());
	if(d->isFinished()) {
		mModel.removeDownload(d);
                mDownloads.take(d);
		delete d;
		d = NULL;
	}
}
/*void DownloadManager::reloadDownload() {
	DownloadData* d = (DownloadData*)sender();
		
	QNetworkReply* oldReply = mDownloads.take(d);
	QFile oldFile = mActiveDownloads.take(oldReply);
	reply.abort()
	oldFile.close();
	oldFile.remove();

	QFile* fileDestination = new QFile(dest);
	fileDestination->open(QIODevice::ReadWrite);
	QNetworkRequest request;
	request.setUrl(d->remotePath());
	QNetworkReply* reply = mNetworkAccess.get(request);
	mActiveDownloads.insert(reply, fileDestination);
	mDownloads.insert(data, reply);
	connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));
}*/
