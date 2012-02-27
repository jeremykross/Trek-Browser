#include "DownloadWidget.h"

DownloadWidget::DownloadWidget() {
	mModel = NULL;
	mStopAction = mReloadAction = mClearAction = mClearAllAction = NULL; 
	initialize();
}

void DownloadWidget::initialize() {
	connect(this, SIGNAL(controller_downloadModelRequested()), Controller::self(), SIGNAL(downloadModelRequested()));
	connect(Controller::self(), SIGNAL(downloadModelReceived(DownloadModel*)), this, SLOT(controller_downloadModelReceived(DownloadModel*)));
	

	mStopAction = mToolBar.addAction("Cancel");
	connect(mStopAction, SIGNAL(triggered()), this, SLOT(stopSelectionTriggered()));
	mClearAction = mToolBar.addAction("Clear");
	connect(mClearAction, SIGNAL(triggered()), this, SLOT(clearSelectionTriggered()));
	mToolBar.addSeparator();
	mClearAllAction = mToolBar.addAction("Clean Up");
	connect(mClearAllAction, SIGNAL(triggered()), this, SLOT(clearAllTriggered()));
	//mToolBar.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	//mToolBar.adjustSize();

	mVMainLayout.addWidget(&mToolBar);
	mVMainLayout.addWidget(&mView); 
	setLayout(&mVMainLayout);


	emit controller_downloadModelRequested();
}
QSortFilterProxyModel* DownloadWidget::proxyModel() {
	return &mProxyModel;
}

void DownloadWidget::controller_downloadModelReceived(DownloadModel* model) {
	mModel = model;
	mProxyModel.setSourceModel(mModel);
	mProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
	mView.setModel(&mProxyModel);
	mView.setItemDelegate(&mDelegate);
	mView.horizontalHeader()->setStretchLastSection(true);
	mView.setSelectionBehavior(QAbstractItemView::SelectRows);
	mView.setSortingEnabled(true);
	connect(mModel, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(setupActions()));
	connect(mView.selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));			
	setupActions();
}

void DownloadWidget::selectionChanged(const QItemSelection& selected, const QItemSelection&) {
	std::cout << "Selection Changed\n";
	QList<QModelIndex> selectedIndexes = selected.indexes();
	mCurrentSelection.clear();
	for(int i=0; i<selectedIndexes.count(); i++) {
		mCurrentSelection.append(mModel->dataFromIndex(selectedIndexes[i]));			
	}
	setupActions();
}

void DownloadWidget::setupActions() {
	if(mCurrentSelection.count() <=0) {
		mStopAction->setEnabled(false);
		mClearAction->setEnabled(false);
		return;	
	}

	bool allDone = true;
	bool allRunning = true;

	for(int i=0; i<mCurrentSelection.count(); i++) {
		if(mCurrentSelection[i]->isFinished()) { allRunning = false;}			
		else {allDone = false;}
	}

	mClearAction->setEnabled(allDone);
	mStopAction->setEnabled(allRunning);
}

void DownloadWidget::clearSelectionTriggered() {
	for(int i=0; i<mCurrentSelection.count(); i++) {
		if(mCurrentSelection[i] && mCurrentSelection[i]->isFinished()) {
			mModel->removeDownload(mCurrentSelection[i]);
		}
	}
}
void DownloadWidget::stopSelectionTriggered() {
	for(int i=0; i<mCurrentSelection.count(); i++) {
		if(mCurrentSelection[i] && mCurrentSelection[i]->isFinished()==false) {
			mCurrentSelection[i]->stop();
		}
	}
}
void DownloadWidget::clearAllTriggered() {
	QList<DownloadData*> toBeRemoved;
	for(int i=0; i<mModel->downloads()->count(); i++) {
		DownloadData* current = (mModel->downloads()->at(i));			
		if(current->isFinished()) {
			toBeRemoved.append(current);
		}
	}
	for(int i=0; i<toBeRemoved.count(); i++) {
		toBeRemoved[i]->remove();
	}
}
