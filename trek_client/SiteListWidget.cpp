#include "SiteListWidget.h"

SiteListWidget::SiteListWidget(UserData* userData, QWidget* parent):QWidget(parent) {
	mModel = NULL;
	mUserData = NULL;
	if(userData) setOwner(userData);
	connect(&mItemTree, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(doubleClicked(const QModelIndex&)));
	connect(this, SIGNAL(urlDoubleClicked(const QUrl&)), Controller::self(), SIGNAL(linkActivated(const QUrl&)));
}

void SiteListWidget::initialize() {
//	mItemTree.setSelectionMode(QAbstractItemView::ExtendedSelection);
	mItemTree.setSelectionBehavior(QAbstractItemView::SelectRows);
	mItemTree.setDragEnabled(true);
	mItemTree.setAcceptDrops(true);
	mItemTree.setAnimated(true);
	//mItemTree.setAcceptDrops(true);
	mItemTree.setDragDropMode(QAbstractItemView::InternalMove);
	mItemTree.setDropIndicatorShown(true);
	mItemTree.setIndentation(15);
	mItemTree.setRootIsDecorated(true);

	mItemTree.header()->setStretchLastSection(true);
	mItemTree.header()->resizeSection(0, 768);

	mMainLayout.addWidget(&mItemTree);
	setLayout(&mMainLayout);	
}

void SiteListWidget::setOwner(UserData* userData) {
	mUserData = userData;
	initialize();
	emit requestSiteListModel(mUserData->guid());
}	

void SiteListWidget::requestData() {
	emit requestSiteListModel(mUserData->guid());
}

SiteListItemModel* SiteListWidget::model() {
	return mModel;
}
QSortFilterProxyModel* SiteListWidget::proxyModel() {
	return &mProxyModel;
}

QTreeView* SiteListWidget::view() {
	return &mItemTree;
}

void SiteListWidget::receiveSiteListModel(const QUuid& guid, SiteListItemModel* model) {
	std::cout << "Widget received model\n";
	if(mUserData->guid() == guid && mModel == NULL) { 
		mModel = model;
		mProxyModel.setSourceModel(mModel);
		mProxyModel.setFilterKeyColumn(-1);
		mProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
		mItemTree.setModel(&mProxyModel);
	}
	std::cout << "Widget done\n";
}

void SiteListWidget::doubleClicked(const QModelIndex& index) {
	if(index.isValid()) {
		QModelIndex sourceIndex = mProxyModel.mapToSource(index);
		SiteListItem* item = static_cast<SiteListItem*>(sourceIndex.internalPointer());
		if(item->url().isValid() && mModel->isEditable() == false) {
			emit urlDoubleClicked(item->url());	
		}
	}
		
}


void SiteListWidgetConnection::setupBookmark(SiteListWidget* widget) {
	QObject::connect(widget, SIGNAL(requestSiteListModel(const QUuid&)), Controller::self(), SIGNAL(favoritesModelRequested(const QUuid&)));
	QObject::connect(Controller::self(), SIGNAL(favoritesModelReceived(const QUuid&, SiteListItemModel*)), widget, SLOT(receiveSiteListModel(const QUuid&, SiteListItemModel*)));
}
void SiteListWidgetConnection::setupHistory(SiteListWidget* widget) {
	QObject::connect(widget, SIGNAL(requestSiteListModel(const QUuid&)), Controller::self(), SIGNAL(historyModelRequested(const QUuid&)));
	QObject::connect(Controller::self(), SIGNAL(historyModelReceived(const QUuid&, SiteListItemModel*)), widget, SLOT(receiveSiteListModel(const QUuid&, SiteListItemModel*)));
}
