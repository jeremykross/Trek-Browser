#include "GraphicsUserListView.h"

GraphicsUserListView::GraphicsUserListView() {
//	mItem1.setDesiredPos(0, DEFAULT_HEIGHT);	

//	mScene.addItem(&mItem);
//	mScene.addItem(&mItem1);
	
//	mItem1.moveVertical(DEFAULT_HEIGHT, 4750); 
//	mItem.moveVertical(DEFAULT_HEIGHT, 5000);
	//mScene.addWidget(new QPushButton("Hello World"));
	setScene(&mScene);
	setAlignment(Qt::AlignLeft|Qt::AlignTop);

//	mScene.addItem(&mSpring);
//	mSpring.setFirstItem(&mItem);
//	mSpring.setSecondItem(&mItem1);
//	mSpring.setBaseLength(DEFAULT_HEIGHT);

//	connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
//	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	setOptimizationFlags(QGraphicsView::DontSavePainterState|QGraphicsView::DontAdjustForAntialiasing);
	//mTimer.start(10);
}

void GraphicsUserListView::resizeEvent(QResizeEvent*) {
	mScene.setSceneRect(0,0,viewport()->width(), mScene.height());
	for(int i=0; i<mUsernames.count(); i++) {
		GraphicsUserListItem* item = mItems.value(mUsernames[i]);
		if(item) item->updateBounds();
	}
//	mItem.updateBounds();
//	mItem1.updateBounds();
}

void GraphicsUserListView::mouseReleaseEvent(QMouseEvent* evt) {
	GraphicsUserListItem* i = (dynamic_cast<GraphicsUserListItem*>(itemAt(evt->pos())));
	if(i) {
		if(evt->button() == Qt::LeftButton) {
			//i->highlight();
			emit highlight(i->userData());
		} else if(evt->button() == Qt::RightButton) {
			emit contextMenu(i->userData()); 
		}
	}
	QGraphicsView::mouseReleaseEvent(evt);
}

void GraphicsUserListView::removeUser(UserData* userData) {
	if(userData) {
		QString id = userData->displayName().toLower();
		mUsernames.removeOne(id);
		GraphicsUserListItem* item = mItems.value(id); 
		if(item) {
			mScene.removeItem(item);
			mItems.remove(id);
			delete item;
			realign();
		}
	}
}

void GraphicsUserListView::addUser(UserData* userData) {
	if(mUsernames.contains(userData->displayName().toLower())) return;
	mUsernames.append(userData->displayName().toLower());	
	GraphicsUserListItem* newItem = new GraphicsUserListItem(userData);
	connect(newItem->selectionsWidget(), SIGNAL(privateChat(UserData*)), this, SIGNAL(privateChat(UserData*)));
	connect(newItem->selectionsWidget(), SIGNAL(profile(UserData*)), this, SIGNAL(profile(UserData*)));
	connect(newItem->selectionsWidget(), SIGNAL(toggleLiked(UserData*)), this, SIGNAL(toggleLiked(UserData*)));
	newItem->setDesiredPos(0, height());
	mScene.addItem(newItem);
	mItems.insert(userData->displayName().toLower(), newItem);

	mUsernames.sort();
	realign();
}

void GraphicsUserListView::realign() {
	int delay = 0;
	for(int i=0; i<mUsernames.count(); i++) {
		GraphicsUserListItem* item = mItems.value(mUsernames[i]);	
		if(item) {
			QPointF newPos(0, i*DEFAULT_HEIGHT);
			if(item->desiredPos() != newPos) {
				item->setDesiredPos(newPos, delay, 450);
				delay += 100;
			}
		}
	}
}

void GraphicsUserListView::addRandomUser() {
	addUser(new UserData());
}
void GraphicsUserListView::removeRandomUser() {
	removeUser(mItems[mUsernames[mUsernames.count()/2]]->userData());
}
void GraphicsUserListView::removeAllUsers() {
	mScene.clear();
	/*QMap<QString, GraphicsUserListItem*>::iterator it;
	for(it = mItems.begin(); it != mItems.end(); ++it) {
		if(it.value()) { 
			delete it.value();
		}
	}*/
	mUsernames.clear();
	mItems.clear();
}
