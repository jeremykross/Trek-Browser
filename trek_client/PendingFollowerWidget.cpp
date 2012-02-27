#include "PendingFollowerWidget.h"

PendingFollowerWidget::PendingFollowerWidget(QWidget* parent):QWidget(parent) {
	mVMainLayout = NULL;
	initialize();	
}
void PendingFollowerWidget::initialize() {
	mOwner = NULL;
	mTreeWidget.setColumnCount(1);
	mTreeWidget.setIconSize(QSize(32,32));
	mTreeWidget.setHeaderLabels(QStringList() << "Pending Relationship Requests"); 
	mIncomingTree.setText(0, "Your Pending Fans:");
	mOutgoingTree.setText(0, "Your Pending Stars (if approved):");
	mTreeWidget.addTopLevelItem(&mIncomingTree);
	mTreeWidget.addTopLevelItem(&mOutgoingTree);
	//mIncomingTree.setDisabled(true);
	//mOutgoingTree.setDisabled(true);

	mNoButton.setText("Deny Checked");
	mYesButton.setText("Allow Checked");
	mHButtonLayout = new QHBoxLayout();
	mHButtonLayout->addWidget(&mYesButton);
	mHButtonLayout->addWidget(&mNoButton);
	mYesButton.hide();
	mNoButton.hide();

	mVMainLayout = new QVBoxLayout();	
	mVMainLayout->addWidget(&mTreeWidget);
	mVMainLayout->addLayout(mHButtonLayout);
	setLayout(mVMainLayout);

	connect(this, SIGNAL(controller_serverAllowFollower(const QUuid&)), Controller::self(), SIGNAL(serverAllowFollower(const QUuid&)));
	connect(this, SIGNAL(controller_serverDenyFollower(const QUuid&)), Controller::self(), SIGNAL(serverDenyFollower(const QUuid&)));
	connect(this, SIGNAL(controller_serverRevokeFollowRequest(const QUuid&)), Controller::self(), SIGNAL(serverRevokeFollowRequest(const QUuid&)));
	connect(this, SIGNAL(controller_showSystemTrayMessage(const QString&, const QString&)), Controller::self(), SIGNAL(showSystemTrayMessage(const QString&, const QString&)));
	connect(this, SIGNAL(controller_userContextMenuRequested(UserData*)), Controller::self(), SIGNAL(userContextMenuRequested(UserData*)));
	connect(Controller::self(), SIGNAL(pendingFollowersReceived(QList<UserData*>)), this, SLOT(controller_pendingFollowersReceived(QList<UserData*>))); 
	connect(Controller::self(), SIGNAL(pendingFollowingReceived(QList<UserData*>)), this, SLOT(controller_pendingFollowingReceived(QList<UserData*>)));
	connect(Controller::self(), SIGNAL(followedReceived(const QUuid&, QList<UserData*>)), this, SLOT(controller_followedReceived(const QUuid&, QList<UserData*>))); 
	connect(&mTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(itemDoubleClicked(QTreeWidgetItem*, int)));
	connect(&mTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(itemPressed(QTreeWidgetItem*, int)));
	connect(&mTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemClicked(QTreeWidgetItem*, int)));

	connect(&mYesButton, SIGNAL(clicked()), this, SLOT(allowChecked()));
	connect(&mNoButton, SIGNAL(clicked()), this, SLOT(denyChecked()));

}

void PendingFollowerWidget::setOwner(UserData* userData) {
	mOwner = userData;
}
	

void PendingFollowerWidget::controller_pendingFollowingReceived(QList<UserData*> userList) {
	for(int i=0; i<userList.count(); i++) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		QVariant v;
		v.setValue(userList[i]);
		item->setData(0, Qt::UserRole, v);
	//	item->setForeground(0, QBrush(Qt::blue));
		item->setText(0, userList[i]->displayName());
		item->setIcon(0, QIcon(userList[i]->icon()));
		mOutgoingTree.addChild(item);
	//	connect(userList[i], SIGNAL(dataAltered()), this, SLOT(helperUpdatePixmaps()));
	}
	//mOutgoingTree.setDisabled(!mOutgoingTree.childCount());
}
void PendingFollowerWidget::controller_pendingFollowersReceived(QList<UserData*> userList) {
	if(userList.count() == 1) {
		emit controller_showSystemTrayMessage(userList.at(0)->displayName(), "Would Like to Become Your Fan.");
	} else {
		emit controller_showSystemTrayMessage("", "You Have Pending Fan Requests.");
	}

	for(int i=0; i<userList.count(); i++) {
		QTreeWidgetItem* item = new QTreeWidgetItem();
		QVariant v;
		v.setValue(userList[i]);
		item->setData(0, Qt::UserRole, v);
		item->setForeground(0, QBrush(Qt::blue));
		item->setText(0, userList[i]->displayName());
		item->setIcon(0, QIcon(userList[i]->icon()));
		item->setCheckState(0, Qt::Unchecked);
		mIncomingTree.addChild(item);
		mIncomingTree.setExpanded(true);
		//connect(userList[i], SIGNAL(dataAltered()), this, SLOT(helperUpdatePixmaps()));
	}
	//mIncomingTree.setDisabled(!mIncomingTree.childCount());
}
void PendingFollowerWidget::controller_followedReceived(const QUuid& owner, QList<UserData*> followed) {
	if(mOwner && owner != mOwner->guid()) return;
	QList<QTreeWidgetItem*> toBeRemoved;
	for(int i=0; i<mOutgoingTree.childCount(); i++) {
		UserData* d = mOutgoingTree.child(i)->data(0, Qt::UserRole).value<UserData*>();	
		if(followed.contains(d)) {
			toBeRemoved.append(mOutgoingTree.child(i));
		}
	}
	if(toBeRemoved.count()) {
		emit controller_showSystemTrayMessage(toBeRemoved.at(0)->text(0), "Is Now One of Your Stars");
	}
	for(int i=0; i<toBeRemoved.count(); i++) {
		mOutgoingTree.removeChild(toBeRemoved.at(i));
		delete toBeRemoved.at(i);
	}
	//mOutgoingTree.setDisabled(!mOutgoingTree.childCount());
}

void PendingFollowerWidget::itemPressed(QTreeWidgetItem* item, int) {
	if(QApplication::mouseButtons() == Qt::RightButton) {
		if(item->parent()) {
			UserData* u = item->data(0,Qt::UserRole).value<UserData*>();
			if(u) 
				emit controller_userContextMenuRequested(u); 
		} else {
			if(item == &mIncomingTree && mIncomingTree.isDisabled()==false) {
				QMenu m;
				QAction* check = m.addAction("Check All");
				QAction* uncheck = m.addAction("Uncheck All");
				connect(check, SIGNAL(triggered()), this, SLOT(checkAll()));
				connect(uncheck, SIGNAL(triggered()), this, SLOT(uncheckAll()));
				m.exec(QCursor::pos());
			}

		}
	}
}
void PendingFollowerWidget::itemClicked(QTreeWidgetItem* item, int) {

	if(item->parent() == NULL) {
	} else if(item->parent() == &mIncomingTree) {
		showButtonsIfNeeded();
	}
}

void PendingFollowerWidget::showButtonsIfNeeded() {
	bool showButtons = false;
	int i=0;
	while(i<mIncomingTree.childCount() && showButtons == false) {
		showButtons = mIncomingTree.child(0)->checkState(0) == Qt::Checked;	
		i++;
	}
	if(showButtons) {
		mYesButton.show();
		mNoButton.show();
	} else {
		mYesButton.hide();
		mNoButton.hide();
	}
}
void PendingFollowerWidget::allow(QTreeWidgetItem* item) {
	UserData* u = item->data(0,Qt::UserRole).value<UserData*>();
	mIncomingTree.removeChild(item);
	delete item;
	emit controller_serverAllowFollower(u->guid());	
	showButtonsIfNeeded();
}
void PendingFollowerWidget::deny(QTreeWidgetItem* item) {
	UserData* u = item->data(0,Qt::UserRole).value<UserData*>();
	mIncomingTree.removeChild(item);
	delete item;
	emit controller_serverDenyFollower(u->guid());
	showButtonsIfNeeded();
}
void PendingFollowerWidget::itemDoubleClicked(QTreeWidgetItem* item, int) {
	UserData* u = item->data(0,Qt::UserRole).value<UserData*>();
	if(item->parent() == NULL) return;
	if(item->parent() == &mIncomingTree) {
		QMessageBox::StandardButton response = QMessageBox::question(this, "Allow Fan Request?", u->displayName() + " would like to become your fan.  Do you allow?", QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if(response == QMessageBox::Yes) {
			allow(item);
		} else if(response == QMessageBox::No) {
			deny(item);
		}
		//mIncomingTree.setDisabled(!mIncomingTree.childCount());
	} else {
		/*QMessageBox::StandardButton response = QMessageBox::question(this, "Revoke Request?", "Would you like to revoke your request to follow " + u->displayName() + "?", QMessageBox::Yes|QMessageBox::No);	
		if(response == QMessageBox::Yes) {
		}*/
	}
}

void PendingFollowerWidget::checkAll() {
	for(int i=0; i<mIncomingTree.childCount(); i++) {
		mIncomingTree.child(i)->setCheckState(0,Qt::Checked);	
	}
	mYesButton.show();
	mNoButton.show();
}
void PendingFollowerWidget::uncheckAll() {
	for(int i=0; i<mIncomingTree.childCount(); i++) {
		mIncomingTree.child(i)->setCheckState(0,Qt::Unchecked);	
	}
	mYesButton.hide();
	mNoButton.hide();
}
void PendingFollowerWidget::denyChecked() {
	if(QMessageBox::question(this, "Deny Checked?", "Are you sure you would like to deny the checked user(s)?", QMessageBox::Yes|QMessageBox::Cancel) == QMessageBox::Cancel) return;
	QList<QTreeWidgetItem*> toDeny;
	for(int i=0; i<mIncomingTree.childCount(); i++) {
		if(mIncomingTree.child(i)->checkState(0) == Qt::Checked) {
			toDeny.append(mIncomingTree.child(i));
		}
	}
	for(int i=0; i<toDeny.count(); i++) {
		deny(toDeny[i]);
	}
}
void PendingFollowerWidget::allowChecked() {
	if(QMessageBox::question(this, "Allow Checked?", "Are you sure you would like to allow the checked user(s)?", QMessageBox::Yes|QMessageBox::Cancel) == QMessageBox::Cancel) return;
	QList<QTreeWidgetItem*> toAllow;
	for(int i=0; i<mIncomingTree.childCount(); i++) {
		if(mIncomingTree.child(i)->checkState(0) == Qt::Checked) {
			toAllow.append(mIncomingTree.child(i));
		}
	}
	for(int i=0; i<toAllow.count(); i++) {
		allow(toAllow[i]);
	}
}

void PendingFollowerWidget::helperUpdatePixmaps() {
	for(int i=0; i<mOutgoingTree.childCount(); i++) {
		QTreeWidgetItem* item = mOutgoingTree.child(i);
		UserData* u = item->data(0,Qt::UserRole).value<UserData*>();
		if(u) {
			item->setIcon(0,u->icon());
		}
	}
	for(int i=0; i<mIncomingTree.childCount(); i++) {
		QTreeWidgetItem* item = mIncomingTree.child(i);
		UserData* u = item->data(0,Qt::UserRole).value<UserData*>();
		if(u) {
			item->setIcon(0,u->icon());
		}
	}
}
