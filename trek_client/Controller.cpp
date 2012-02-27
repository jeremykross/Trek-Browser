#include "Controller.h"

Controller* Controller::mInstance = NULL;

Controller* Controller::self() {
	if (mInstance == NULL) {
		mInstance = new Controller;
	}
	return mInstance;
}

GraphicsAvatarItem* Controller::constructUserAvatar() {
	UserData* userData = new UserData();
	userData->setDisplayName("Jeremy S. Kross");
	userData->setIcon(QPixmap("/home/jkross/Pictures/misc/me1.jpg"));
	GraphicsAvatarItem* item = new GraphicsAvatarItem(userData);
//	item->setFlag(QGraphicsItem::ItemIsSelectable);
//	item->setFlag(QGraphicsItem::ItemIsMovable);
        new GraphicsSpeechBalloonItem(item, userData);
	return item;
}

void Controller::helperAddAvatar(UserData* userData) {
	emit avatarAdded(userData); 
}	
