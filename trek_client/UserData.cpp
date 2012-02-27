#include "UserData.h"

bool UserData::stDefaultsInitialized = false;
QMap<QString, QVariant> UserData::stDefaultValues;

UserData::UserData() {
	if(stDefaultsInitialized == false) {
		initializeDefaultValues();
	}
	setIcon(stDefaultValues.value("icon").value<QPixmap>());
	mHasIcon = false;
	mDisplayName = stDefaultValues.value("displayName").toString();
	mUsernameBGColor = stDefaultValues.value("usernameBGColor").value<QColor>();
	mSpeechBalloonBGColor = stDefaultValues.value("speechBalloonBGColor").value<QColor>();
	mUsernameFontColor = stDefaultValues.value("usernameFontColor").value<QColor>();
	mSpeechBalloonFontColor = stDefaultValues.value("speechBalloonFontColor").value<QColor>();
	
	mFont.setPointSize(10);
	mFont.setBold(false);
	mStatus = OFFLINE;
//	mFont.setFamily("purisa");
}

void UserData::initializeDefaultValues() {
	if(stDefaultsInitialized) return;
        stDefaultValues.insert("icon", QPixmap(":/icons/questionmark.jpg"));
	stDefaultValues.insert("displayName", "anonymous");
	stDefaultValues.insert("usernameBGColor", QColor("cornflowerblue").darker());
	stDefaultValues.insert("speechBalloonBGColor", QColor("cornflowerblue").darker());
	stDefaultValues.insert("usernameFontColor", QColor("cadetblue"));
	stDefaultValues.insert("speechBalloonFontColor", QColor("white"));
	stDefaultsInitialized = true;
}


QPixmap UserData::icon() {
	return mIcon;
}
QPixmap UserData::scaledIcon() {
	return mScaledIcon;
}
void UserData::setIcon(QPixmap pixmap) {
	mIcon = pixmap.scaled(256,256, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	mScaledIcon = pixmap.scaled(PIXMAP_DIMENSION, PIXMAP_DIMENSION, Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
	mHasIcon = true;
}

bool UserData::hasIcon() {
	return mHasIcon; 
}


QString UserData::displayName() {
	return mDisplayName;
}

void UserData::setDisplayName(QString displayName) {
	mDisplayName = displayName;
}

QFont UserData::font() {
	return mFont;
}
void UserData::setFont(QFont font) {
	mFont = font;
}

QColor UserData::usernameBGColor() {
	return mUsernameBGColor;
}

void UserData::setUsernameBGColor(QColor color) {
	mUsernameBGColor = color;
}

QColor UserData::balloonBGColor() {
	return mSpeechBalloonBGColor;
}
void UserData::setBalloonBGColor(QColor color) {
	mSpeechBalloonBGColor = color;
}

//Font colors
QColor UserData::balloonFontColor() {
	return mSpeechBalloonFontColor;	
}
void UserData::setBalloonFontColor(QColor color) {
	mSpeechBalloonFontColor = color;
}
QColor UserData::usernameFontColor() {
	return mUsernameFontColor;
}
void UserData::setUsernameFontColor(QColor color) {
	mUsernameFontColor = color;
}

void UserData::setGuid(QUuid guid) {
	mGuid = guid;
}
QUuid UserData::guid() {
	return mGuid;
}

void UserData::setPartyColor(const QColor& c, bool isLeader) {
	QColor color = c;
	color.setAlpha(172);
	mPartyColor = color;
	mIsLeader = isLeader;
}
QColor UserData::partyColor() {
	return mPartyColor;
}
bool UserData::isLeader() {
	return mIsLeader;
}

void UserData::setStatus(Status s) {
	mStatus = s;
}
Status UserData::status() {
	return mStatus;
}
QString UserData::statusAsString() {
	if(mStatus == ONLINE) {
		return "Online";
	} else {
		return "Offline";
	}
}

void UserData::setPersonalMessage(const QString& message) {
	mMessage = message;
}
QString UserData::personalMessage() {
	return mMessage;
}

void UserData::setLocaleUrl(const QString& localeUrl) {
	mUrl = localeUrl;
}
QString UserData::localeUrlAsString() {
	return mUrl;
}

void UserData::altered() {
	emit dataAltered(); 
}
void UserData::informStatusAltered() {
	emit statusAltered();
}


/*bool UserData::operator<(const UserData* other) {
	std::cout << "Operating\n";
	if(mDisplayName.compare(other->mDisplayName)<0) {
		return true;	
	} else {
		return false;	
	}
}*/
