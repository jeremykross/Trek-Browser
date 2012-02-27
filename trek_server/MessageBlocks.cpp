#include "MessageBlocks.h"

//MessageType Message::stType = NO_OP;
//MessageType AuthenticationMessage::stType = AUTHENTICATION;

Message* MessageFactory::getMessageByType(unsigned short type) {
	switch(type) {
		case AUTHENTICATION:
			return new AuthenticationMessage();
			break;
		case UNIVERSAL_REQUEST:
			return new UniversalRequestMessage();
			break;
		case s2cAUTHENTICATED:
			return new s2cAuthenticatedMessage();
		case c2sSPEAK:
			return new c2sSpeakMessage();
		case c2sPERSONAL_SPEAK:
			return new c2sPersonalSpeakMessage();
		case APPEARANCE:
			return new AppearanceMessage();
		case c2sREQUEST_APPEARANCE:
			return new c2sRequestAppearanceMessage();
		case c2sLOCALE_CHANGED:
			return new c2sLocaleChangedMessage();
		case s2cADD_USERS:
			return new s2cAddUsersMessage();
		case s2cADD_FOLLOWED:
			return new s2cAddFollowedMessage();
		case s2cADD_FANS:
			return new s2cAddFansMessage();
		case s2cREMOVE_FANS:
			return new s2cRemoveFansMessage();
		case s2cREMOVE_FOLLOWED:
			return new s2cRemoveFollowedMessage();
		case c2sFOLLOW_USER:
			return new c2sFollowUserMessage();
		case c2sUNFOLLOW_USER:
			return new c2sUnfollowUserMessage();
		case s2cLISTEN:
			return new s2cListenMessage();
		case s2cPERSONAL_LISTEN:
			return new s2cPersonalListenMessage();
		case s2cREMOVE_USERS:
			return new s2cRemoveUsersMessage();
		case s2cPROFILE:
			return new s2cProfileMessage();
		case c2sSIGNUP:
			return new c2sSignupMessage();
		case s2cSIGNUP_RESPONSE:
			return new s2cSignupResponseMessage();
		case c2sCHANGE_APPEARANCE:
			return new c2sChangeAppearanceMessage();
		case c2sCHANGE_PROFILE:
			return new c2sChangeProfileMessage();
		case c2sCHANGE_MOTD:
			return new c2sChangeMotdMessage();
		case c2sMOVE:
			return new c2sMoveMessage();
		case s2cMOVE:
			return new s2cMoveMessage();
		case s2cSTATUS:
			return new s2cStatusMessage();
		case s2cSAY:
			return new s2cSayMessage();
		case c2sADD_BOOKMARKS:
			return new c2sAddBookmarksMessage();
		case c2sREMOVE_BOOKMARKS:
			return new c2sRemoveBookmarksMessage();
		case s2cADD_BOOKMARKS:
			return new s2cAddBookmarksMessage();
		case c2sADD_HISTORY:
			return new c2sAddHistoryMessage();
		case s2cADD_HISTORY:
			return new s2cAddHistoryMessage();
		case c2sASK_JOIN:
			return new c2sAskJoinMessage();
		case c2sASK_UNJOIN:
			return new c2sAskUnjoinMessage();
		case s2cPARTY_MOVE:
			return new s2cPartyMoveMessage();
		case s2cPARTY_ADD:
			return new s2cPartyAddUsersMessage();
		case s2cPARTY_REMOVE:
			return new s2cPartyRemoveUsersMessage();
		case s2cNONCE:
			return new s2cNonceMessage();
		case c2sBETA_INVITE:
			return new c2sBetaInviteMessage();
		case s2cINVITE_COUNT:
			return new s2cInviteCountMessage();
		case s2cPENDING_FOLLOWERS:
			return new s2cPendingFollowersMessage();
		case s2cPENDING_FOLLOWING:
			return new s2cPendingFollowingMessage();
		case c2sCONF_KEY:
			return new c2sConfKeyMessage();
		case c2sRESET_PASSWORD:
			return new c2sResetPasswordMessage();
		case c2sCHANGE_PASSWORD:
			return new c2sChangePasswordMessage();
#ifdef ADMIN
		case c2sADMIN_COMMAND:
			return new c2sAdminCommandMessage();
#endif
		default:
			return new Message();
	}
}

/*-----------------------------------------------------------*/

Message::Message() {
	mType = NO_OP;
	blockSize = 0;
}

bool Message::enoughDataPresent(QIODevice* device) {
	QDataStream in(device);
	if(blockSize == 0) {
		if(device->bytesAvailable() < sizeof(quint16)) {
			return false;
		} 
		in >> blockSize;
	}

	if(blockSize > 65000) {
		std::cout << "Large: " << blockSize << "\n";
	}

	if(device->bytesAvailable() < blockSize) {
		std::cout << QString("Waiting for more data: %1 present, %2 needed\n").arg(device->bytesAvailable()).arg(blockSize).toStdString();
		return false;
	}
	return true;
}


bool Message::read(QIODevice* device, int position) {
	QDataStream in(device);
	derivedRead(in);
	return true;
}

bool Message::write(QIODevice* device,int position, bool prependStats) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	if(prependStats) {
		out << quint16(mType) << quint32(0);
	}
	derivedWrite(out);
	if(prependStats) {
		out.device()->seek(2);
		out << quint32(block.size()-(sizeof(quint16)+sizeof(quint32)));
	}
#ifdef CLIENT
	if(block.size() < 65530) //MAX_quint16 65,535
		device->write(block);
	else {
		std::cout << QString("Error: Block Size To Large to Send: %1 bytes\n").arg(block.size()).toStdString();
	}
#else
	device->write(block);
#endif
	
	return true;
}

void Message::print() {
	std::cout << "No custom print defined for type: " << mType << "\n";
}

/*------------------------------------------*/
c2sBetaInviteMessage::c2sBetaInviteMessage() {
	mType = c2sBETA_INVITE;
}
void c2sBetaInviteMessage::derivedRead(QDataStream& in) {
	in >> mToEmail >> mFromName;
}
void c2sBetaInviteMessage::derivedWrite(QDataStream& out) {
	out << mToEmail << mFromName;
}	

/*--------------------------------------------*/
s2cInviteCountMessage::s2cInviteCountMessage() {
	mType = s2cINVITE_COUNT;
}
void s2cInviteCountMessage::derivedRead(QDataStream& in) {
	in >> mCount;
}
void s2cInviteCountMessage::derivedWrite(QDataStream& out) {
	out << mCount;
}

/*------------------------------------------*/
s2cNonceMessage::s2cNonceMessage() {
	mType = s2cNONCE;
}
void s2cNonceMessage::derivedRead(QDataStream& in) {
	in >> mNonce; 
}
void s2cNonceMessage::derivedWrite(QDataStream& out) {
	out << mNonce;
}
void s2cNonceMessage::print() {
	std::cout << "NONCE\n";
}

/*------------------------------------------*/
QString AuthenticationMessage::stNonce;
QString AuthenticationMessage::stUsername;
QString AuthenticationMessage::stPassword;

AuthenticationMessage::AuthenticationMessage() {
	mType = AUTHENTICATION;
}
void AuthenticationMessage::derivedRead(QDataStream& in) {
	in >> mUsername >> mPassword;
	mUsername =mUsername.toLower();
}

void AuthenticationMessage::derivedWrite(QDataStream& out) {
	out << mUsername << mPassword;
}

void AuthenticationMessage::print() {
	std::cout << "[AuthenticationMessage] "<< "Username: " << mUsername.toStdString() << "  Password: " << mPassword.toStdString() << "\n";
}

/*------------------------------------------*/

s2cAuthenticatedMessage::s2cAuthenticatedMessage() {
	mType = s2cAUTHENTICATED;
	mIsSuccessful = false;
}

void s2cAuthenticatedMessage::derivedRead(QDataStream& in) {
	in >> mIsSuccessful >> mGuid >> mUsername >> mCause;
//	mUsername = mUsername.toLower();
}

void s2cAuthenticatedMessage::derivedWrite(QDataStream& out) {
	out << mIsSuccessful << mGuid << mUsername << mCause;
}

void s2cAuthenticatedMessage::print() {
	std::cout << "[s2cAuthenticatedMessage] Authentication ";
	if(mIsSuccessful) std::cout << "Successful. Guid: " << mGuid.toString().toStdString()<<"\n";
	else              std::cout << "Failed.\n";
}

/*----------------------------------------------*/
c2sSpeakMessage::c2sSpeakMessage() { 
	mType = c2sSPEAK;	
}

void c2sSpeakMessage::derivedRead(QDataStream& in) {
	in >> mCommunique;
}

void c2sSpeakMessage::derivedWrite(QDataStream& out) {
	out << mCommunique;
}

void c2sSpeakMessage::print() {
	std::cout << "[c2sSpeakMessage] The client said, \"" << mCommunique.toStdString() << "\"\n";
}

/*------------------------------------------------------------*/
c2sPersonalSpeakMessage::c2sPersonalSpeakMessage() {
	mType = c2sPERSONAL_SPEAK; 
}

void c2sPersonalSpeakMessage::derivedRead(QDataStream& in) {
	in >> mGuid >> mCommunique; 
}
void c2sPersonalSpeakMessage::derivedWrite(QDataStream& out) {
	out << mGuid << mCommunique;
}

/*-----------------------------------------------------*/
s2cPersonalListenMessage::s2cPersonalListenMessage() {
	mType = s2cPERSONAL_LISTEN; 
}

/*----------------------------------------------------------*/

s2cListenMessage::s2cListenMessage() {
	mType = s2cLISTEN;
}

void s2cListenMessage::derivedRead(QDataStream& in) {
	in >> mGuid >> mCommunique;
}
void s2cListenMessage::derivedWrite(QDataStream& out) {
	out << mGuid << mCommunique;
}

void s2cListenMessage::print() {
	std::cout << "[s2cListenMessage] " << mGuid.toString().toStdString() << " said, \"" << mCommunique.toStdString() << "\"\n";
}

/*------------------------------------------------------------------*/

AppearanceMessage::AppearanceMessage() {
	mType = APPEARANCE;
	hasIcon = hasUsername = hasUsernameBGColor = hasSpeechBalloonBGColor = hasUsernameFontColor = hasSpeechBalloonFontColor = hasPartyColor = mIsLeader = false;
}

void AppearanceMessage::print() {
	std::cout << "[AppearanceMessage] Appearance delivered for guid " << mGuid.toString().toStdString() << "\n";
}

void AppearanceMessage::derivedRead(QDataStream& in) {
	in >> hasIcon >> hasUsername >> hasUsernameBGColor >> hasSpeechBalloonBGColor >> hasUsernameFontColor >> hasSpeechBalloonFontColor >> hasPartyColor;
	in >> mGuid;
	if(hasIcon)                   in >> mIcon;
	if(hasUsername)               in >> mUsername;
	if(hasUsernameBGColor)        in >> mUsernameBGColor;
	if(hasSpeechBalloonBGColor)   in >> mSpeechBalloonBGColor;
	if(hasUsernameFontColor)      in >> mUsernameFontColor;
	if(hasSpeechBalloonFontColor) in >> mSpeechBalloonFontColor;
	if(hasPartyColor)           { in >> mPartyColor; in >> mIsLeader; }
}

void AppearanceMessage::derivedWrite(QDataStream& out) {
	out << hasIcon << hasUsername << hasUsernameBGColor << hasSpeechBalloonBGColor << hasUsernameFontColor << hasSpeechBalloonFontColor << hasPartyColor;
	out << mGuid;
	if(hasIcon)                   out << mIcon;
	if(hasUsername)               out << mUsername;
	if(hasUsernameBGColor)        out << mUsernameBGColor;
	if(hasSpeechBalloonBGColor)   out << mSpeechBalloonBGColor;
	if(hasUsernameFontColor)      out << mUsernameFontColor;
	if(hasSpeechBalloonFontColor) out << mSpeechBalloonFontColor;
	if(hasPartyColor)           { out << mPartyColor; out << mIsLeader; }
}

/*------------------------------------------------------------*/

c2sRequestAppearanceMessage::c2sRequestAppearanceMessage() {
	mType = c2sREQUEST_APPEARANCE;
	requestIcon = requestUsername = requestUsernameBGColor = requestSpeechBalloonBGColor = requestUsernameFontColor = requestSpeechBalloonFontColor = false;
}

void c2sRequestAppearanceMessage::print() {
	std::cout << "[c2sRequestAppearanceMessage] Appearance requested for guid " << mGuid.toString().toStdString();
}

void c2sRequestAppearanceMessage::derivedRead(QDataStream& in) {
	in >>requestIcon >> requestUsername >> requestUsernameBGColor >> requestSpeechBalloonBGColor >> requestUsernameFontColor >> requestSpeechBalloonFontColor >> mGuid;
}	
void c2sRequestAppearanceMessage::derivedWrite(QDataStream& out) {
	out << requestIcon << requestUsername << requestUsernameBGColor << requestSpeechBalloonBGColor << requestUsernameFontColor << requestSpeechBalloonFontColor <<  mGuid;
}	

/*--------------------------------------------------------*/

c2sLocaleChangedMessage::c2sLocaleChangedMessage() {
	mType = c2sLOCALE_CHANGED;
}
void c2sLocaleChangedMessage::print() {
	std::cout << "[c2sLocaleChangedMessage] New Locale: " << mNewUrl.toString().toStdString() << ".\n";
}
void c2sLocaleChangedMessage::derivedRead(QDataStream& in) {
	in >> mOldUrl >> mNewUrl;
}
void c2sLocaleChangedMessage::derivedWrite(QDataStream& out) {
	out << mOldUrl << mNewUrl;
}

/*----------------------------------------------------------------*/
s2cAddUsersMessage::s2cAddUsersMessage() {
	mType = s2cADD_USERS;
}
void s2cAddUsersMessage::print() {
	std::cout << "[s2cAddUsersMessage]\n";
	for(int i=0; i<mUsersCount; i++) {
		std::cout << "Guid: " << mGuids[i].toString().toStdString() << " with Username: " << mUsernames[i].toStdString() << "\n";
	}
}

void s2cAddUsersMessage::derivedRead(QDataStream& in) {
	in >> mUsersCount >> mGuids >> mUsernames;
}	

void s2cAddUsersMessage::derivedWrite(QDataStream& out) {
	out << mUsersCount << mGuids << mUsernames;
}

/*-------------------------------------------------------------*/
void s2cAddRelatedUsersMessage::derivedRead(QDataStream& in) {
	in >> mOwnerGuid;
	s2cAddUsersMessage::derivedRead(in);
}
void s2cAddRelatedUsersMessage::derivedWrite(QDataStream& out) {
	out << mOwnerGuid;
	s2cAddUsersMessage::derivedWrite(out);
}

/*---------------------------------------------------------*/
s2cAddFollowedMessage::s2cAddFollowedMessage() {
	mType = s2cADD_FOLLOWED;
}

/*----*/

s2cAddFriendsMessage::s2cAddFriendsMessage() {
	mType = s2cADD_FRIENDS;
}	
/*----*/
s2cAddFansMessage::s2cAddFansMessage() {
	mType = s2cADD_FANS;
}
/*------------*/
s2cRemoveFansMessage::s2cRemoveFansMessage() {
	mType = s2cREMOVE_FANS;
}
s2cRemoveFollowedMessage::s2cRemoveFollowedMessage() {
	mType = s2cREMOVE_FOLLOWED;
}
/*----------------*/
s2cPendingFollowersMessage::s2cPendingFollowersMessage() {
	mType = s2cPENDING_FOLLOWERS;
}
/*--*/
s2cPendingFollowingMessage::s2cPendingFollowingMessage() {
	mType = s2cPENDING_FOLLOWING;
}

/*-----------------------------------------------------*/
c2sRequestFriendsMessage::c2sRequestFriendsMessage() {
	//mType = c2sREQUEST_FRIENDS;
}
void c2sRequestFriendsMessage::derivedRead(QDataStream& in) {
	in >> mGuid;
}
void c2sRequestFriendsMessage::derivedWrite(QDataStream& out) {
	out << mGuid;
}

/*--------------------------------------------------*/

c2sFollowUserMessage::c2sFollowUserMessage() {
	mType = c2sFOLLOW_USER;
}

void c2sFollowUserMessage::derivedRead(QDataStream& in) {
	in >> mFriendGuid;
}	
void c2sFollowUserMessage::derivedWrite(QDataStream& out) {
	out << mFriendGuid;
}
/*----------------------------------------------------*/

c2sUnfollowUserMessage::c2sUnfollowUserMessage() {
	mType = c2sUNFOLLOW_USER;
}

/*----------------------------------------------------*/

s2cRemoveUsersMessage::s2cRemoveUsersMessage() {
	mType = s2cREMOVE_USERS;
}

void s2cRemoveUsersMessage::print() {
	std::cout << "[s2cRemoveUsersMessage]\n";
	for(int i=0; i<mGuids.count(); i++) {
		std::cout << "Guid: " << mGuids[i].toString().toStdString() << "\n";
	}
}

void s2cRemoveUsersMessage::derivedRead(QDataStream& in) {
	in >> mUsersCount >> mGuids >> mUrls;
}

void s2cRemoveUsersMessage::derivedWrite(QDataStream& out) {
	out << mUsersCount << mGuids << mUrls;
}
/*----------------------------------------------------------------*/
c2sSignupMessage::c2sSignupMessage() {
	mType = c2sSIGNUP;
}
void c2sSignupMessage::print() {
	std::cout << "[c2sSignupMessage]\n";
	std::cout << "Username " << mDesiredUsername.toStdString() << "\n";
}
void c2sSignupMessage::derivedRead(QDataStream& in) {
	in >> mEmailAddress >> mFirstName >> mLastName >> mSex >> mBirthDate >> mDesiredUsername >> mPassword >> mKey;
	mEmailAddress = mEmailAddress.toLower();
}
void c2sSignupMessage::derivedWrite(QDataStream& out) {
	out << mEmailAddress << mFirstName << mLastName << mSex << mBirthDate << mDesiredUsername << mPassword << mKey;
}

/*-------------------------------------------------*/
s2cSignupResponseMessage::s2cSignupResponseMessage() {
	mType = s2cSIGNUP_RESPONSE; 
	mSuccessful = false;
}
void s2cSignupResponseMessage::print() {
	std::cout << "[s2cSignupResponseMessage]\n";
	if(mSuccessful) std::cout << "Signup was successful.\n";
	else            std::cout << "Signup was failed.\n";
}
void s2cSignupResponseMessage::derivedRead(QDataStream& in) {
	in >> mSuccessful >> mMesg;
}
void s2cSignupResponseMessage::derivedWrite(QDataStream& out) {
	out << mSuccessful << mMesg;
}

/*----------------------------------------------------------*/
s2cProfileMessage::s2cProfileMessage() {
	mType = s2cPROFILE;
}

void s2cProfileMessage::derivedRead(QDataStream& in) {
	in >> mGuid >> mEmailAddress >> mGender >> mBirthDate >> mOccupationInfo >> mHobbies >> mFavoriteCulture >> mGeneralInfo >> mMotd;
	mEmailAddress = mEmailAddress.toLower();
} 
void s2cProfileMessage::derivedWrite(QDataStream& out) {
	out << mGuid << mEmailAddress << mGender << mBirthDate << mOccupationInfo << mHobbies << mFavoriteCulture << mGeneralInfo << mMotd;
} 

/*---------------------------------------------------------*/
c2sChangeProfileMessage::c2sChangeProfileMessage() {
	mType = c2sCHANGE_PROFILE;
}

void c2sChangeProfileMessage::derivedRead(QDataStream& in) {
	in >> mOccupationInfo >> mHobbies >> mFavoriteCulture >> mGeneralInfo;	
}
void c2sChangeProfileMessage::derivedWrite(QDataStream& out){
	out << mOccupationInfo << mHobbies << mFavoriteCulture << mGeneralInfo;	
}

/*-------------------------------*/
c2sChangeMotdMessage::c2sChangeMotdMessage() {
	mType = c2sCHANGE_MOTD;
}
void c2sChangeMotdMessage::derivedRead(QDataStream& in) {
	in >> mMessage;
}
void c2sChangeMotdMessage::derivedWrite(QDataStream& out) {
	out << mMessage;
}


/*---------------------------------------------------------*/

c2sChangeAppearanceMessage::c2sChangeAppearanceMessage() {
	mType = c2sCHANGE_APPEARANCE;
}
void c2sChangeAppearanceMessage::print() {
	std::cout << "[s2cChangeAppearanceMessage]\n";
}
void c2sChangeAppearanceMessage::derivedRead(QDataStream& in) {
	in >> mScaledPixmap >> mUsernameBGColor >> mSpeechBalloonBGColor >> mUsernameFontColor >> mSpeechBalloonFontColor;
}
void c2sChangeAppearanceMessage::derivedWrite(QDataStream& out) {
	out << mScaledPixmap << mUsernameBGColor << mSpeechBalloonBGColor << mUsernameFontColor << mSpeechBalloonFontColor;
}
/*---------------------------------------------------------*/

MoveMessage::MoveMessage() {}

void MoveMessage::print() {
	std::cout << "[MoveMessage]\n";
}
void MoveMessage::derivedRead(QDataStream& in) {
	in >> mId >> mLocationX >> mLocationY >> mElementX >> mElementY >> mElementWidth >> mElementHeight >> mBodyWidth >> mBodyHeight;
}
void MoveMessage::derivedWrite(QDataStream& out) {
	out << mId << mLocationX << mLocationY << mElementX << mElementY << mElementWidth << mElementHeight << mBodyWidth << mBodyHeight;
}

/*---                                     ---*/

c2sMoveMessage::c2sMoveMessage() {
	mType = c2sMOVE;
}
/*---                                      ---*/

s2cMoveMessage::s2cMoveMessage() {
	mType = s2cMOVE;
}
void s2cMoveMessage::derivedRead(QDataStream& in) {
	in >> mGuid;
	MoveMessage::derivedRead(in);
}
void s2cMoveMessage::derivedWrite(QDataStream& out) {
	out << mGuid;
	MoveMessage::derivedWrite(out);
}

/*-------------------------------------------------------------------*/
s2cStatusMessage::s2cStatusMessage() {
	mType = s2cSTATUS;	
}
void s2cStatusMessage::derivedRead(QDataStream& in) {
	in >> mGuid >> mStatus >> mUrl >> mMessage;
}
void s2cStatusMessage::derivedWrite(QDataStream& out) {
	out << mGuid << mStatus << mUrl << mMessage;
}
/*--------------------*/
s2cSayMessage::s2cSayMessage() {
	mType = s2cSAY;
}
void s2cSayMessage::derivedRead(QDataStream& in) {
	in >> mMessage;
}
void s2cSayMessage::derivedWrite(QDataStream& out) {
	out << mMessage;
}

/*-------------------------------------------------------------------*/
SiteMessage::SiteMessage() {}
void SiteMessage::derivedRead(QDataStream& in) {
	in >> mNames >> mUrls;
}
void SiteMessage::derivedWrite(QDataStream& out) {
	out << mNames << mUrls;
}
/*------------------------------------------------------------------------*/
c2sAddBookmarksMessage::c2sAddBookmarksMessage() {
	mType = c2sADD_BOOKMARKS;
}
/*--------------------------------------------------------------------------*/
c2sRemoveBookmarksMessage::c2sRemoveBookmarksMessage() {
	mType = c2sREMOVE_BOOKMARKS;
}
/*---------------------------------------------------*/
s2cAddBookmarksMessage::s2cAddBookmarksMessage() {
	mType = s2cADD_BOOKMARKS;
};
void s2cAddBookmarksMessage::derivedRead(QDataStream& in) {
	in >> mGuid;
	SiteMessage::derivedRead(in);
}
void s2cAddBookmarksMessage::derivedWrite(QDataStream& out) {
	out << mGuid;
	SiteMessage::derivedWrite(out);
}

/*-------------------------------------------*/
#ifdef ADMIN
c2sAdminCommandMessage::c2sAdminCommandMessage() {
	mType = c2sADMIN_COMMAND;
}
void c2sAdminCommandMessage::derivedRead(QDataStream& in) {
	in >> mUsername >> mCommand >> mOption;
}
void c2sAdminCommandMessage::derivedWrite(QDataStream& out) {
	out << mUsername << mCommand << mOption;
}
#endif
/*-------------------------------------------*/
c2sChangePasswordMessage::c2sChangePasswordMessage() {
	mType = c2sCHANGE_PASSWORD;
}
void c2sChangePasswordMessage::derivedRead(QDataStream& in) {
	in >> mNewPass;
}
void c2sChangePasswordMessage::derivedWrite(QDataStream& out) {
	out << mNewPass;
}
/*-----------------------------------------------*/
c2sResetPasswordMessage::c2sResetPasswordMessage() {
	mType = c2sRESET_PASSWORD;
}
void c2sResetPasswordMessage::derivedRead(QDataStream& in) {
	in >> mEmailAddress;
}
void c2sResetPasswordMessage::derivedWrite(QDataStream& out) {
	out << mEmailAddress;
}
/*------------------------------------------------*/
c2sConfKeyMessage::c2sConfKeyMessage() {
	mType = c2sCONF_KEY;
}
void c2sConfKeyMessage::derivedRead(QDataStream& in) {
	in >> mKey;
}
void c2sConfKeyMessage::derivedWrite(QDataStream& out) {
	out << mKey;
}
/*------------------------------------------------*/

c2sAskJoinMessage::c2sAskJoinMessage() {
	mType = c2sASK_JOIN;
}
void c2sAskJoinMessage::derivedRead(QDataStream& in) {
	in>> mGuids >> mColor;
}
void c2sAskJoinMessage::derivedWrite(QDataStream& out) {
	out << mGuids << mColor;
}

/*--------------*/

c2sAskUnjoinMessage::c2sAskUnjoinMessage() {
	mType =c2sASK_UNJOIN;
}

/*---------------------------------------*/
s2cPartyAddUsersMessage::s2cPartyAddUsersMessage() {
	mType = s2cPARTY_ADD; 
}
/*-----------------*/
s2cPartyRemoveUsersMessage::s2cPartyRemoveUsersMessage() {
	mType = s2cPARTY_REMOVE;
}
/*---------------------------*/
s2cPartyMoveMessage::s2cPartyMoveMessage() {
	mType = s2cPARTY_MOVE;
}

void s2cPartyMoveMessage::derivedRead(QDataStream& in) {
	in >> mUrl;
}
void s2cPartyMoveMessage::derivedWrite(QDataStream& out) {
	out << mUrl;
}

/*------------------------------------------------------------------------*/
UniversalRequestMessage::UniversalRequestMessage() {
	mType = UNIVERSAL_REQUEST;
}
void UniversalRequestMessage::derivedRead(QDataStream& in) {
	in >> mGuid >> mRequest;
}
void UniversalRequestMessage::derivedWrite(QDataStream& out) {
	out << mGuid << mRequest;
}
void UniversalRequestMessage::print() {
	std::cout << "Universal: " << mRequest.toStdString() << "\n";
}
