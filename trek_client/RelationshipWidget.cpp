#include "RelationshipWidget.h"

RelationshipWidget::RelationshipWidget() {
	mModel = NULL;	
	mOwner = NULL;

	//setContextMenuPolicy(Qt::CustomContextMenu);

	mVMainLayout.addWidget(&mView);
	setLayout(&mVMainLayout);

	connect(&mView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(catchDoubleClick(const QModelIndex&)));
	connect(this, SIGNAL(controller_userContextMenuRequested(UserData*)), Controller::self(), SIGNAL(userContextMenuRequested(UserData*)));
	connect(this, SIGNAL(controller_userDoubleClicked(UserData*)), Controller::self(), SIGNAL(userDoubleClicked(UserData*)));
}	
void RelationshipWidget::setUserData(UserData* data) {
	mOwner = data;

}

void RelationshipWidget::setViewFollowed() {
	connect(this, SIGNAL(controller_modelRequested(const QUuid&)), Controller::self(), SIGNAL(followedModelRequested(const QUuid&)));
	connect(Controller::self(), SIGNAL(followedModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_modelOffered(const QUuid&, RelationshipListModel*)));
	emit controller_modelRequested(mOwner->guid());
}

void RelationshipWidget::setViewFans() {
	connect(this, SIGNAL(controller_modelRequested(const QUuid&)), Controller::self(), SIGNAL(fansModelRequested(const QUuid&)));
	connect(Controller::self(), SIGNAL(fansModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_modelOffered(const QUuid&, RelationshipListModel*)));
	emit controller_modelRequested(mOwner->guid());
}

QSortFilterProxyModel* RelationshipWidget::proxyModel() {
	return &mProxyModel;
}

void RelationshipWidget::controller_modelOffered(const QUuid& guid, RelationshipListModel* model) {
	if(guid == mOwner->guid()) {
		if(mModel == NULL) {
			mModel = model;
			mProxyModel.setSourceModel(mModel);	
			mProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
			mView.setModel(&mProxyModel);
			//mView.setFlow(QListView::LeftToRight);
			mView.setViewMode(QListView::IconMode);
			mView.setWrapping(true);
			mView.setResizeMode(QListView::Adjust);
			mView.setSpacing(10);
			mView.clearSelection();
			mProxyModel.sort(0, Qt::AscendingOrder);
			disconnect(Controller::self(), SIGNAL(followedModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_modelOffered(const QUuid&, RelationshipListModel*)));
		}
	}		
}

UserData* RelationshipWidget::currentSelection() {
	QItemSelection selection = mView.selectionModel()->selection();	
	QModelIndex i = mProxyModel.mapToSource(selection.indexes().value(0));
	return mModel->userDataFromIndex(i);
}

void RelationshipWidget::catchDoubleClick(const QModelIndex& index) {
	if(mModel) {
		UserData* d = mModel->userDataFromIndex(mProxyModel.mapToSource(index));
		if(d) {
			emit doubleClicked(d);
		}
	}
}
void RelationshipWidget::contextMenuEvent(QContextMenuEvent*) {
	UserData* data = currentSelection();
	if(data)
		emit controller_userContextMenuRequested(data);	
}

/*--------------------------------------------------*/
CompoundRelationshipWidget::CompoundRelationshipWidget(QWidget* parent):QTabWidget(parent) {
	mOwner=NULL;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(&mFollowedWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SIGNAL(customContextMenuRequested(const QPoint&)));
	connect(&mFansWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SIGNAL(customContextMenuRequested(const QPoint&)));
	connect(&mFollowedWidget, SIGNAL(doubleClicked(UserData*)), this, SIGNAL(doubleClicked(UserData*)));
	connect(&mFansWidget, SIGNAL(doubleClicked(UserData*)), this, SIGNAL(doubleClicked(UserData*)));

}

void CompoundRelationshipWidget::setOwner(UserData* owner) {
	mOwner = owner;
	mFollowedWidget.setUserData(mOwner);
	mFollowedWidget.setViewFollowed();
	mFansWidget.setUserData(mOwner);
	mFansWidget.setViewFans();

	addTab(&mFollowedWidget, "My Stars");
	addTab(&mFansWidget, "My Fans");

}

RelationshipWidget* CompoundRelationshipWidget::followedWidget() {
	return &mFollowedWidget;	
}
RelationshipWidget* CompoundRelationshipWidget::fansWidget() {
	return &mFansWidget;
}

UserData* CompoundRelationshipWidget::currentSelection() {	
	if(currentWidget() == &mFollowedWidget) return mFollowedWidget.currentSelection();
	else if (currentWidget() == &mFansWidget) return mFansWidget.currentSelection();
	else /* ? */ return NULL;	
}

