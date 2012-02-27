#include "DownloadData.h"

DownloadData::DownloadData() {
	mRemotePath = "No/Path/Specified";
	mLocalPath = "No/Path/Specified";
	mBytesReceived = 0;
	mBytesTotal = -1;
	mTransferRate = -1;
	mIsFinished = false;
	mIsStopped = false;

}

bool DownloadData::isFinished() {
	return mIsFinished;
}
bool DownloadData::isStopped() {
	return mIsStopped;
}

void DownloadData::finished() {
	mIsFinished = true;
	emit done();
}

void DownloadData::stop() {
	emit stopMe();
}
void DownloadData::remove() {
	emit removeMe();
}

QString DownloadData::remotePath() {
	return mRemotePath;
}
QString DownloadData::localPath() {
	return mLocalPath;
}
QString DownloadData::localName() {
	return QFileInfo(mLocalPath).fileName();
}

qint64 DownloadData::bytesReceived() {
	return mBytesReceived;
}
qint64 DownloadData::bytesTotal() {
	return mBytesTotal;
}

qreal DownloadData::percentageComplete() {
	return (qreal)mBytesReceived/mBytesTotal;
}

void DownloadData::catchProgress(qint64 bytesReceived, qint64 bytesTotal) {
	mBytesReceived = bytesReceived;
	mBytesTotal = bytesTotal;	
	emit progress(bytesReceived, bytesTotal);
}
