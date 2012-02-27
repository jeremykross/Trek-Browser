#include "UserDataFactory.h"

UserDataFactory* UserDataFactory::stInstance = NULL;

UserDataFactory::UserDataFactory() {
	connect(this, SIGNAL(controller_serverRequestAppearance(const QUuid&, bool,bool,bool,bool,bool,bool)), Controller::self(), SIGNAL(serverRequestAppearance(const QUuid&, bool,bool,bool,bool,bool,bool))); 
	connect(this, SIGNAL(controller_serverRequestStatus(const QUuid&)), Controller::self(), SIGNAL(serverRequestStatus(const QUuid&)));
	connect(Controller::self(), SIGNAL(avatarAppearanceChanged(const QUuid&, const QPixmap&, const QString&, const QColor&, const QColor&, const QColor&, const QColor&)), this, SLOT(controller_avatarAppearanceChanged(const QUuid&, const QPixmap&, const QString&, const QColor&, const QColor&, const QColor&, const QColor&)));
	connect(this, SIGNAL(controller_userOnline(UserData*)), Controller::self(), SIGNAL(userOnline(UserData*)));
	connect(this, SIGNAL(controller_userOffline(UserData*)), Controller::self(), SIGNAL(userOffline(UserData*)));

	connect(Controller::self(), SIGNAL(statusUpdated(const QUuid&, Status, const QString&, const QString&)), this, SLOT(controller_statusUpdated(const QUuid&, Status, const QString&, const QString&)));
}

UserDataFactory::~UserDataFactory() {
	QMap<QUuid, UserData*>::iterator i;
	for(i=mKnownUserData.begin(); i!= mKnownUserData.end(); i++) {
		UserData* curr = i.value();
		if(curr) {
			delete curr;
			curr = NULL;
		}
	}
}

UserDataFactory* UserDataFactory::self() {
	if(stInstance == NULL) {
		stInstance = new UserDataFactory;
	}
	return stInstance;
}

void UserDataFactory::controller_avatarAppearanceChanged(const QUuid& guid, const QPixmap& pixmap, const QString& userName, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& balloonFontColor) {
	if(mKnownUserData.contains(guid)) {
		UserData* d = mKnownUserData.value(guid);
		if(pixmap.isNull() == false) {
			d->setIcon(pixmap);
		}
		if(userName.isEmpty() == false)
			d->setDisplayName(userName);
		if(usernameBGColor.isValid()) 
			d->setUsernameBGColor(usernameBGColor);
		if(usernameFontColor.isValid()) 
			d->setUsernameFontColor(usernameFontColor);
		if(balloonBGColor.isValid()) 
			d->setBalloonBGColor(balloonBGColor);
		if(balloonFontColor.isValid())
			d->setBalloonFontColor(balloonFontColor);
	
		d->altered();
	}
}

bool UserDataFactory::isUserDataKnown(const QUuid& guid) {
	return mKnownUserData.value(guid);
}

UserData* UserDataFactory::userDataForGuid(const QUuid& guid) {
	if(mKnownUserData.contains(guid)) {
		return mKnownUserData.value(guid);
	} else {
		UserData* u = new UserData();
		u->setGuid(guid);
		mKnownUserData.insert(guid, u);
		emit controller_serverRequestAppearance(guid, true,true,true,true,true,true); 
		emit controller_serverRequestStatus(guid);
		return u;
	}	
}
/*
void UserDataFactory::requestAppearance(const QUuid& guid) {
	emit controller_serverRequestAppearance(guid, true,true,true,true,true,true);}
void UserDataFactory::requestStatus(const QUuid& guid) {
	emit controller_requestStatus(guid);
}
*/
void UserDataFactory::controller_statusUpdated(const QUuid& guid, Status status, const QString& mesg, const QString& url) {	
	UserData* d= mKnownUserData.value(guid);	
	if(d) {
		Status oldStatus = d->status();
		d->setStatus(status);	
		d->setPersonalMessage(mesg);	
		d->setLocaleUrl(url);
		d->informStatusAltered();

		if(oldStatus == OFFLINE && status == ONLINE) {
			emit controller_userOnline(d);
		} else if(oldStatus == ONLINE && status == OFFLINE) {
			emit controller_userOffline(d);
		}
	}
}

