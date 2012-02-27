#include "MessageCodec.h"

MessageCodec::MessageCodec() {}

/* I'm also setting up various miscelaneous signal/slot connections here not directly related to the ServerConnection*, this isn't great coding practice; I dunno why I started lumping all that stuff in here, probably accidental, really.*/
void MessageCodec::registerServerConnection(ServerConnection* serverConnection) {
	connect(serverConnection, SIGNAL(messageReceived(Message*)), this, SLOT(receiveMessage(Message*)));	
	connect(this, SIGNAL(sendMessage(Message*)), serverConnection, SLOT(sendMessage(Message*)));

	connect(this, SIGNAL(serverAuthenticationSuccess(UserData*)), Controller::self(), SIGNAL(serverAuthenticationSuccess(UserData*)));
	connect(this, SIGNAL(serverAuthenticationFailure(const QString&)), Controller::self(), SIGNAL(serverAuthenticationFailure(const QString&)));
	connect(this, SIGNAL(controller_pendingFollowersReceived(QList<UserData*>)), Controller::self(), SIGNAL(pendingFollowersReceived(QList<UserData*>)));
	connect(this, SIGNAL(controller_pendingFollowingReceived(QList<UserData*>)), Controller::self(), SIGNAL(pendingFollowingReceived(QList<UserData*>)));
	connect(this, SIGNAL(controller_showSystemTrayMessage(const QString&, const QString&)), Controller::self(), SIGNAL(showSystemTrayMessage(const QString&, const QString&)));
	//connect(this, SIGNAL(controller_requestConfirmationKey()), Controller::self(), SIGNAL(requestConfirmationKey()));
	connect(Controller::self(), SIGNAL(serverAuthenticationAttempted(const QString&, const QString&)), this, SLOT(authenticate(const QString&, const QString&)));
	//Look here!
	connect(Controller::self(), SIGNAL(serverSignupAttempted(const QString&, const QString&, const QString&, const QString&,const QDate&, const QString&, const QString&, const QString&)), this, SLOT(signup(const QString&, const QString&, const QString&, const QString&,const QDate&, const QString&, const QString&, const QString&))); 
	connect(Controller::self(), SIGNAL(serverUpdateAppearance(UserData*)), this, SLOT(updateAppearance(UserData*)));
		
	connect(Controller::self(), SIGNAL(serverPersonalSpeak(const QUuid&, const QString&)), this, SLOT(personalSpeak(const QUuid&, const QString&))); 

	connect(Controller::self(), SIGNAL(serverSpeak(const QString&)), this, SLOT(speak(const QString&)));
	connect(Controller::self(), SIGNAL(serverChangeLocale(QUrl, QUrl)), this, SLOT(changeLocale(QUrl, QUrl)));
	
	connect(Controller::self(), SIGNAL(serverRequestAppearance(const QUuid&, bool, bool, bool, bool, bool, bool)), this, SLOT(requestAppearance(const QUuid&, bool, bool, bool, bool, bool, bool)));

	connect(Controller::self(), SIGNAL(serverChangePosition(unsigned int, int, int, int, int, int, int,int,int)), this, SLOT(move(unsigned int, int,int,int,int,int,int,int,int)));

	connect(Controller::self(), SIGNAL(serverRequestFollow(const QUuid&)), this, SLOT(requestFollow(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverRequestUnfollow(const QUuid&)), this, SLOT(requestUnfollow(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverRequestFollowed(const QUuid&)), this, SLOT(requestFollowed(const QUuid&)));

	connect(Controller::self(), SIGNAL(serverRequestFans(const QUuid&)), this, SLOT(requestFans(const QUuid&)));
	//connect(Controller::self(), SIGNAL(serverRequestFriends(const QUuid&)), this, SLOT(requestFriends(const QUuid&)));

	connect(Controller::self(), SIGNAL(serverRequestStatus(const QUuid&)), this, SLOT(requestStatus(const QUuid&)));


	connect(Controller::self(), SIGNAL(addFavorite(const QString&, const QUrl&)), this, SLOT(controller_addFavorite(const QString&, const QUrl&)));
	connect(Controller::self(), SIGNAL(removeFavorite(const QString&, const QUrl&)), this, SLOT(controller_removeFavorite(const QString&, const QUrl&)));
	connect(Controller::self(), SIGNAL(logHistory(const QString&, const QUrl&)), this, SLOT(controller_logHistory(const QString&, const QUrl&)));

	connect(Controller::self(), SIGNAL(serverUpdateProfile(const QString&, const QString&, const QString&, const QString&)), this, SLOT(changeProfile(const QString&, const QString&, const QString&, const QString&)));
	connect(Controller::self(), SIGNAL(serverChangePersonalMotd(const QString&)), this, SLOT(controller_serverChangePersonalMotd(const QString&)));
	connect(Controller::self(), SIGNAL(serverClearHistory()), this, SLOT(controller_serverClearHistory()));
	connect(Controller::self(), SIGNAL(serverRequestProfile(const QUuid&)), this, SLOT(requestProfile(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverRequestFavorites(const QUuid&)), this, SLOT(controller_serverRequestFavorites(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverRequestHistory(const QUuid&)), this, SLOT(controller_serverRequestHistory(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverRequestLocations()), this, SLOT(controller_serverRequestLocations()));
	connect(Controller::self(), SIGNAL(serverKick(const QUuid&)), this, SLOT(controller_serverKick(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverInviteToExpedition(const QColor&, QList<UserData*>)), this, SLOT(controller_serverInviteToExpedition(const QColor&, QList<UserData*>)));
	connect(Controller::self(), SIGNAL(serverUninviteFromExpedition(QList<UserData*>)), this, SLOT(controller_serverUninviteFromExpedition(QList<UserData*>)));
	connect(Controller::self(), SIGNAL(serverExpeditionRequestResponse(const QUuid&, bool)), this, SLOT(controller_serverExpeditionRequestResponse(const QUuid&, bool))); 
	//connect(Controller::self(), SIGNAL(serverBetaInvite(const QString&, const QString&)), this, SLOT(controller_serverBetaInvite(const QString&, const QString&)));
	//connect(Controller::self(), SIGNAL(serverGetInviteCount()), this, SLOT(controller_serverGetInviteCount()));
	connect(Controller::self(), SIGNAL(serverRequestPendingFollowers()), this, SLOT(controller_serverRequestPendingFollowers()));
	connect(Controller::self(), SIGNAL(serverRequestPendingFollowing()), this, SLOT(controller_serverRequestPendingFollowing()));
	connect(Controller::self(), SIGNAL(serverAllowFollower(const QUuid&)), this, SLOT(controller_serverAllowFollower(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverDenyFollower(const QUuid&)), this, SLOT(controller_serverDenyFollower(const QUuid&)));
	connect(Controller::self(), SIGNAL(serverBlock(const QUuid&)), this, SLOT(controller_serverBlock(const QUuid&)));
	//connect(Controller::self(), SIGNAL(serverConfirmKey(const QString&)), this, SLOT(controller_serverConfirmKey(const QString&)));
	connect(Controller::self(), SIGNAL(serverRequestPasswordReset()), this, SLOT(controller_serverRequestPasswordReset()));
	connect(Controller::self(), SIGNAL(serverChangePassword(const QString&)), this, SLOT(controller_serverChangePassword(const QString&)));
#ifdef ADMIN
	connect(Controller::self(), SIGNAL(serverExecuteAdminCommand(const QString&, int, const QString&)), this, SLOT(controller_serverExecuteAdminCommand(const QString&, int, const QString&)));
#endif

	//connect(this, SIGNAL(controller_confKeyValid(bool)), Controller::self(), SIGNAL(confKeyValid(bool)));
	connect(this, SIGNAL(addUser(UserData*)), Controller::self(), SIGNAL(avatarAdded(UserData*)));
	connect(this, SIGNAL(removeUser(const QUuid&, const QUrl&)), Controller::self(), SIGNAL(avatarRemoved(const QUuid&, const QUrl&)));
	connect(this, SIGNAL(communiqueReceived(const QUuid&, const QString&)), Controller::self(), SIGNAL(communiqueReceived(const QUuid&, const QString&)));
	connect(this, SIGNAL(personalCommuniqueReceived(const QUuid&, const QString&)), Controller::self(), SIGNAL(personalCommuniqueReceived(const QUuid&, const QString&)));

	connect(this, SIGNAL(signupSuccess()), Controller::self(), SIGNAL(serverSignupSuccess()));
	connect(this, SIGNAL(signupFailure(const QString&)), Controller::self(), SIGNAL(serverSignupFailure(const QString&)));

	connect(this, SIGNAL(alterAppearance(const QUuid&, const QPixmap&, const QString&, const QColor&, const QColor&, const QColor&, const QColor&)), Controller::self(), SIGNAL(avatarAppearanceChanged(const QUuid&, const QPixmap&, const QString&, const QColor&, const QColor&, const QColor&, const QColor&)));
	connect(this, SIGNAL(alterPartyAppearance(const QUuid&, const QColor&, bool)), Controller::self(), SIGNAL(avatarPartyAppearanceChanged(const QUuid&, const QColor&, bool)));
	connect(this, SIGNAL(positionChanged(const QUuid&, unsigned int, float, float)), Controller::self(), SIGNAL(avatarPositionChanged(const QUuid&, unsigned int, float, float)));
	connect(this, SIGNAL(positionChangedBackup(const QUuid&, float, float)), Controller::self(), SIGNAL(avatarPositionChangedBackup(const QUuid&, float, float)));

	connect(this, SIGNAL(addFollowed(const QUuid&, QList<UserData*>)), Controller::self(), SIGNAL(followedReceived(const QUuid&, QList<UserData*>)));
	connect(this, SIGNAL(addFans(const QUuid&, QList<UserData*>)), Controller::self(), SIGNAL(fansReceived(const QUuid&, QList<UserData*>)));
	connect(this, SIGNAL(controller_serverRemoveFans(const QUuid&, QList<UserData*>)), Controller::self(), SIGNAL(serverRemoveFans(const QUuid&, QList<UserData*>)));
	connect(this, SIGNAL(controller_serverRemoveFollowed(const QUuid&, QList<UserData*>)), Controller::self(), SIGNAL(serverRemoveFollowed(const QUuid&, QList<UserData*>)));
	connect(this, SIGNAL(updateStatus(const QUuid&, Status, const QString&, const QString&)), Controller::self(), SIGNAL(statusUpdated(const QUuid&, Status, const QString&, const QString&)));
	connect(this, SIGNAL(updateProfile(const QUuid&,const QString&, const QString&, const QDate&, const QString&, const QString&, const QString&, const QString&)), Controller::self(), SIGNAL(profileUpdated(const QUuid&,const QString&, const QString&, const QDate&, const QString&, const QString&, const QString&, const QString&)));
	connect(this, SIGNAL(controller_updateMotd(const QString&)), Controller::self(), SIGNAL(updateMotd(const QString&)));
	connect(this, SIGNAL(controller_favoritesDataReceived(const QUuid&, QList<SiteData*>)), Controller::self(), SIGNAL(favoritesDataReceived(const QUuid&, QList<SiteData*>)));
	connect(this, SIGNAL(controller_historyDataReceived(const QUuid&, QList<SiteData*>)), Controller::self(), SIGNAL(historyDataReceived(const QUuid&, QList<SiteData*>)));
	connect(this, SIGNAL(controller_askingExpeditionJoin(const QUuid&)), Controller::self(), SIGNAL(askingExpeditionJoin(const QUuid&)));
	connect(this, SIGNAL(controller_partyMove(const QUrl&)), Controller::self(), SIGNAL(partyMove(const QUrl&)));
	connect(this, SIGNAL(controller_partyAddUser(UserData*)), Controller::self(), SIGNAL(partyAddUser(UserData*)));
	connect(this, SIGNAL(controller_partyRemoveUser(const QUuid&)), Controller::self(), SIGNAL(partyRemoveUser(const QUuid&)));
	connect(this, SIGNAL(controller_partyEjected()), Controller::self(), SIGNAL(partyEjected()));
	//connect(this, SIGNAL(controller_inviteCountReceived(int)), Controller::self(), SIGNAL(inviteCountReceived(int)));

}

void MessageCodec::initialize() {}


void MessageCodec::receiveMessage(Message* mesg) {
	if(mesg->mType == s2cAUTHENTICATED) {
		processAuthenticated((s2cAuthenticatedMessage*)mesg);
	} else if(mesg->mType == s2cADD_USERS) {
		processAddUsers((s2cAddUsersMessage*)mesg);
	} else if (mesg->mType == s2cREMOVE_USERS) {
		processRemoveUsers((s2cRemoveUsersMessage*)mesg);
	} else if(mesg->mType == s2cADD_FOLLOWED) {
		processAddFollowed((s2cAddFollowedMessage*)mesg);
	} else if(mesg->mType == s2cADD_FANS) {
		processAddFans((s2cAddFansMessage*)mesg);
	} else if(mesg->mType == s2cREMOVE_FANS) {
		processRemoveFans((s2cRemoveFansMessage*)mesg);
	} else if(mesg->mType == s2cREMOVE_FOLLOWED) {
		processRemoveFollowed((s2cRemoveFollowedMessage*) mesg);
	} else if(mesg->mType == s2cADD_FRIENDS) {

	} else if(mesg->mType == s2cLISTEN) {
		processListen((s2cListenMessage*)mesg);
	} else if(mesg->mType == s2cPERSONAL_LISTEN) {
		processPersonalListen((s2cPersonalListenMessage*)mesg);
	} else if(mesg->mType == s2cSIGNUP_RESPONSE) {
		processSignupResponse((s2cSignupResponseMessage*)mesg);
	} else if(mesg->mType == APPEARANCE) {
		processAppearance((AppearanceMessage*)mesg);
	} else if(mesg->mType == s2cMOVE) {
		processMove((s2cMoveMessage*)mesg);
	} else if(mesg->mType == s2cSTATUS) {
		processStatus((s2cStatusMessage*)mesg);
	} else if(mesg->mType == s2cPROFILE) {
		processProfile((s2cProfileMessage*)mesg);
	} else if(mesg->mType == s2cADD_BOOKMARKS) {
		processAddBookmarks((s2cAddBookmarksMessage*)mesg);
	} else if(mesg->mType == s2cADD_HISTORY) {
		processAddHistory((s2cAddHistoryMessage*)mesg);
	} else if(mesg->mType == s2cPARTY_MOVE) {
		processPartyMove((s2cPartyMoveMessage*)mesg);
	} else if(mesg->mType == s2cPARTY_ADD) {
		processPartyAddUsers((s2cPartyAddUsersMessage*)mesg);
	} else if(mesg->mType == s2cPARTY_REMOVE) {
		processPartyRemoveUsers((s2cPartyRemoveUsersMessage*)mesg);;
	} else if(mesg->mType == s2cNONCE) {
		processNonce((s2cNonceMessage*)mesg);
	} else if(mesg->mType == s2cPENDING_FOLLOWERS) {
		processPendingFollowers((s2cPendingFollowersMessage*)mesg);
	} else if(mesg->mType == s2cPENDING_FOLLOWING) {
		processPendingFollowing((s2cPendingFollowingMessage*)mesg);
	} else if(mesg->mType == s2cINVITE_COUNT) {
		//processInviteCount((s2cInviteCountMessage*)mesg);
	} else if(mesg->mType == s2cSAY) {
		processSay((s2cSayMessage*)mesg); 
	} else if(mesg->mType == UNIVERSAL_REQUEST) {
		processUniversalRequest((UniversalRequestMessage*)mesg);
	} else {
		QMessageBox::information(NULL, "Network Error", "Unknown message type.");
	}
}

void MessageCodec::processNonce(s2cNonceMessage* mesg) {
	std::cout << "Receieved nonce\n";
	AuthenticationMessage authMesg;
	authMesg.mUsername = AuthenticationMessage::stUsername;
	QString password;
	if(AuthenticationMessage::stPassword.startsWith("reset_")) {
		password = AuthenticationMessage::stPassword;
	} else {
		password = QString::fromAscii(QCryptographicHash::hash(AuthenticationMessage::stPassword.toAscii(), QCryptographicHash::Sha1));
	}
	std::cout << "Pure Password: " << AuthenticationMessage::stPassword.toStdString() << "\n";
	std::cout << "Password on Disk: " << password.toStdString() << "\n";
	std::cout << "Nonce: " << mesg->mNonce.toStdString() << "\n";
	authMesg.mPassword = QString(QCryptographicHash::hash((password+mesg->mNonce).toAscii(), QCryptographicHash::Sha1));
	emit sendMessage(&authMesg);
	std::cout << "Sent Pass " << authMesg.mPassword.toStdString() << "\n";
}
void MessageCodec::processPendingFollowers(s2cPendingFollowersMessage* mesg) {
	QList<UserData*> userList;
	for(int i=0; i<mesg->mGuids.size(); i++) {
		UserData* ud = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]);	
		ud->setDisplayName(mesg->mUsernames[i]);
		userList.append(ud);
	}
	emit controller_pendingFollowersReceived(userList);
}
void MessageCodec::processPendingFollowing(s2cPendingFollowingMessage* mesg) {
	QList<UserData*> userList;
	for(int i=0; i<mesg->mGuids.size(); i++) {
		UserData* ud = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]);	
		ud->setDisplayName(mesg->mUsernames[i]);
		userList.append(ud);
	}
	emit controller_pendingFollowingReceived(userList);
}
/*void MessageCodec::processInviteCount(s2cInviteCountMessage* mesg) {
	emit controller_inviteCountReceived(mesg->mCount);	
}*/
void MessageCodec::processSay(s2cSayMessage* mesg) {
//	if(mesg->mMessage.startsWith("Motd:") == false) {
//		mesg->mMessage.prepend("Server Says: ");
//	}	
	emit controller_showSystemTrayMessage("", mesg->mMessage);
}

void MessageCodec::processAuthenticated(s2cAuthenticatedMessage* mesg) {
	if(mesg->mIsSuccessful) {
		UserData* userData = UserDataFactory::self()->userDataForGuid(mesg->mGuid); 
		userData->setDisplayName(mesg->mUsername);
		emit serverAuthenticationSuccess(userData);
	} else {
		std::cout << "Emitting failiure\n";
		emit serverAuthenticationFailure(mesg->mCause);
	}
}
void MessageCodec::processListen(s2cListenMessage* mesg) {
	emit communiqueReceived(mesg->mGuid, mesg->mCommunique); 	
}
void MessageCodec::processPersonalListen(s2cPersonalListenMessage* mesg) {
	emit personalCommuniqueReceived(mesg->mGuid, mesg->mCommunique);	
}

void MessageCodec::processAddUsers(s2cAddUsersMessage* mesg) {
	for(int i=0; i<mesg->mUsersCount; i++) {
		UserData* userData = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]); //new UserData();
		userData->setDisplayName(mesg->mUsernames[i]);
		emit addUser(userData);
	}

}
void MessageCodec::processRemoveUsers(s2cRemoveUsersMessage* mesg) {
	for(int i=0; i<mesg->mUsersCount; i++) {
		emit removeUser(mesg->mGuids[i], mesg->mUrls[i]);
	}
}

void MessageCodec::processAddFans(s2cAddFansMessage* mesg) {
	QList<UserData*> data;
	for(int i=0; i<mesg->mGuids.count(); i++) {
		UserData* ud = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]);
		//ud->setGuid(mesg->mGuids[i]);
		//ud->setDisplayName(mesg->mUsernames[i]);	
		data.append(ud);
	}
	emit addFans(mesg->mOwnerGuid,data);
}

void MessageCodec::processRemoveFollowed(s2cRemoveFollowedMessage* mesg) {
	QList<UserData*> data;
	for(int i=0; i<mesg->mGuids.count(); i++) {
		UserData* ud = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]);
		data.append(ud);
	}
	emit controller_serverRemoveFollowed(mesg->mOwnerGuid, data); 
}
void MessageCodec::processRemoveFans(s2cRemoveFansMessage* mesg) {
	QList<UserData*> data;
	for(int i=0; i<mesg->mGuids.count(); i++) {
		UserData* ud = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]);
		data.append(ud);
	}
	emit controller_serverRemoveFans(mesg->mOwnerGuid, data); 
}

void MessageCodec::processAddFollowed(s2cAddFollowedMessage* mesg) {
	QList<UserData*> data;
	for(int i=0; i<mesg->mGuids.count(); i++) {
		UserData* ud = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]);
		//ud->setGuid(mesg->mGuids[i]);
		//ud->setDisplayName(mesg->mUsernames[i]);	
		data.append(ud);
	}
	emit addFollowed(mesg->mOwnerGuid,data);
}
void MessageCodec::processSignupResponse(s2cSignupResponseMessage* mesg) {
	if(mesg->mSuccessful) emit signupSuccess();
	else                  emit signupFailure(mesg->mMesg);

}

void MessageCodec::processAppearance(AppearanceMessage* mesg) {
	//Fill me in
	QUuid guid = mesg->mGuid;
	QPixmap pixmap;
	QString username;
	QColor usernameBGColor, usernameFontColor, balloonBGColor, balloonFontColor;

	if(mesg->hasIcon) {
		pixmap = QPixmap::fromImage(mesg->mIcon);
	}
	if(mesg->hasUsername) {
		username = mesg->mUsername;
	}
	if(mesg->hasUsernameBGColor) {
		usernameBGColor = mesg->mUsernameBGColor;
	}
	if(mesg->hasUsernameFontColor) {
		usernameFontColor = mesg->mUsernameFontColor;
	}
	if(mesg->hasSpeechBalloonBGColor) {
		balloonBGColor = mesg->mSpeechBalloonBGColor;
	}
	if(mesg->hasSpeechBalloonFontColor) {
		balloonFontColor = mesg->mSpeechBalloonFontColor;
	}
	
	emit alterAppearance(guid, pixmap, username, usernameBGColor, usernameFontColor, balloonBGColor, balloonFontColor);
	
	if(mesg->hasPartyColor) {
		emit alterPartyAppearance(guid, mesg->mPartyColor, mesg->mIsLeader);
	} else {
		emit alterPartyAppearance(guid, QColor(), false);
	}
}

void MessageCodec::processMove(s2cMoveMessage* mesg) {

	int distanceX = mesg->mLocationX - mesg->mElementX;
	int distanceY = mesg->mLocationY - mesg->mElementY;

	emit positionChanged(mesg->mGuid, mesg->mId, (float)distanceX/(float)mesg->mElementWidth, (float)distanceY/(float)mesg->mElementHeight); 

	emit positionChangedBackup(mesg->mGuid, (float)mesg->mLocationX/(float)mesg->mBodyWidth, (float)mesg->mLocationY/(float)mesg->mBodyHeight);
}

void MessageCodec::processStatus(s2cStatusMessage* mesg) {
	Status s = OFFLINE;
	if(mesg->mStatus == ONLINE) {
		s = ONLINE;
	} else if(mesg->mStatus == OFFLINE) {
		s = OFFLINE;	
	}
	emit updateStatus(mesg->mGuid, s, mesg->mMessage, mesg->mUrl);	
}
void MessageCodec::processProfile(s2cProfileMessage* mesg) {
	emit updateProfile(mesg->mGuid,mesg->mEmailAddress, mesg->mGender, mesg->mBirthDate, mesg->mOccupationInfo, mesg->mHobbies, mesg->mFavoriteCulture, mesg->mGeneralInfo);
	emit controller_updateMotd(mesg->mMotd);
}
void MessageCodec::processAddBookmarks(s2cAddBookmarksMessage* mesg) {
	QList<SiteData*> dataList;
	for(int i=0; i<mesg->mNames.count(); i++) {
		SiteData* data = new SiteData();
		data->mName = mesg->mNames[i];
       		data->mUrl = mesg->mUrls[i];
		dataList.append(data);
	}	
	emit controller_favoritesDataReceived(mesg->mGuid,dataList);
}

void MessageCodec::processAddHistory(s2cAddHistoryMessage* mesg) {
	QList<SiteData*> dataList;
        for(int i=0; i<mesg->mNames.count(); i++) {
		SiteData* data = new SiteData();
		data->mName = mesg->mNames[i];
        	data->mUrl = mesg->mUrls[i];
		dataList.append(data);
	}	
	emit controller_historyDataReceived(mesg->mGuid,dataList);
}


void MessageCodec::processPartyAddUsers(s2cPartyAddUsersMessage* mesg) {
	for(int i=0; i<mesg->mUsersCount; i++) {
		UserData* userData = UserDataFactory::self()->userDataForGuid(mesg->mGuids[i]); //new UserData();
		userData->setDisplayName(mesg->mUsernames[i]);
		emit controller_partyAddUser(userData);
	}
}

void MessageCodec::processPartyRemoveUsers(s2cPartyRemoveUsersMessage* mesg) {
	for(int i=0; i<mesg->mGuids.count(); i++) {
		emit controller_partyRemoveUser(mesg->mGuids[i]);
	}
}
void MessageCodec::processPartyMove(s2cPartyMoveMessage* mesg) {
	emit controller_partyMove(mesg->mUrl);
}

void MessageCodec::processUniversalRequest(UniversalRequestMessage* mesg) {

	if(mesg->mRequest.startsWith("expedition_ask")) {
                //UserData* d = UserDataFactory::self()->userDataForGuid(mesg->mGuid);
		emit controller_askingExpeditionJoin(mesg->mGuid);
	} else if(mesg->mRequest.compare("expedition_unask") == 0) {
		emit controller_partyEjected();	
	} else if(mesg->mRequest.compare("auth_key") == 0) {
		//emit controller_requestConfirmationKey(); 
	} else if(mesg->mRequest.compare("auth_valid") == 0) {
		//emit controller_confKeyValid(true);
	} else if(mesg->mRequest.compare("auth_invalid") ==0) {
		//emit controller_confKeyValid(false);
	}
}

void MessageCodec::authenticate(const QString& username, const QString& password) {
	/*QSettings settings("Jeremy Kross Soft", "Trek");
	QString salt = settings.value("salt/"+username).toString(); 
	QString saltedPass;
	saltedPass.append(salt);
	saltedPass.append(password);
	std::cout << "Authenticating: " << saltedPass.toStdString() << "\n";
	QCryptographicHash cryptoHash(QCryptographicHash::Md5);
	cryptoHash.addData(saltedPass.toAscii()); */

	AuthenticationMessage tempAuth;
	tempAuth.stUsername = username;
	tempAuth.stPassword = password;

	std::cout << "Requesting Nonce\n";
	UniversalRequestMessage reqNonce;
	reqNonce.mRequest = "nonce";
	emit sendMessage(&reqNonce);

	/*AuthenticationMessage mesg;
	mesg.mUsername = username;
	mesg.mPassword = password;//QString(cryptoHash.result());
	emit sendMessage(&mesg);*/
}

void MessageCodec::speak(const QString& communique) {
	c2sSpeakMessage mesg;
	mesg.mCommunique = communique;
	emit sendMessage(&mesg);
}

void MessageCodec::personalSpeak(const QUuid& recipient, const QString& communique) {
	c2sPersonalSpeakMessage mesg;
	mesg.mGuid = recipient;
	mesg.mCommunique = communique;
	emit sendMessage(&mesg);
}

void MessageCodec::move(unsigned int id, int locationX, int locationY, int elementX, int elementY, int elementWidth, int elementHeight, int bodyWidth, int bodyHeight) {
	c2sMoveMessage mesg;
	mesg.mId = id;
	mesg.mLocationX     = locationX;
	mesg.mLocationY     = locationY;
	mesg.mElementX      = elementX;
	mesg.mElementY      = elementY;
	mesg.mElementWidth  = elementWidth;
	mesg.mElementHeight = elementHeight;
	mesg.mBodyWidth = bodyWidth;
	mesg.mBodyHeight = bodyHeight; 
	emit sendMessage(&mesg);
}

void MessageCodec::changeLocale(QUrl oldUrl, QUrl newUrl) {
	c2sLocaleChangedMessage mesg;
	mesg.mOldUrl = oldUrl;
	mesg.mNewUrl = newUrl;
	emit sendMessage(&mesg); 
}

void MessageCodec::signup(const QString& emailAddress, const QString& firstName, const QString& lastName, const QString& sex,const QDate& date, const QString& desiredUsername, const QString& password, const QString& authKey) { 
	c2sSignupMessage mesg;
	mesg.mEmailAddress = emailAddress;
	mesg.mFirstName = firstName;
	mesg.mLastName = lastName;
	mesg.mSex = sex;
	mesg.mBirthDate = date; 
	mesg.mDesiredUsername = desiredUsername;
	mesg.mKey = authKey;
	//Hash the password
	/*srand(time(NULL));
	QString allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	QString salt;
	salt.append(allowedChars.at(rand()%allowedChars.count()));
	salt.append(allowedChars.at(rand()%allowedChars.count()));
	salt.append(allowedChars.at(rand()%allowedChars.count()));
	salt.append(allowedChars.at(rand()%allowedChars.count()));
	QString saltedPass;
	saltedPass.append(salt);
	saltedPass.append(password);
	QSettings settings("Jeremy Kross Soft", "Trek");
	settings.setValue("salt/"+emailAddress, salt); 
	std::cout << "Salting: " << saltedPass.toStdString() << "\n";
	QCryptographicHash cryptoHash(QCryptographicHash::Md5);
	cryptoHash.addData(saltedPass.toAscii()); */
	mesg.mPassword = password; //QString(QCryptographicHash::hash(password.toAscii(), QCryptographicHash::Sha1)); //QString(cryptoHash.result());
	/*std::cout << "Hashed Pass: " << mesg.mPassword.toStdString() << "\n";
	QMessageBox::information(NULL, "Hashed Pass", mesg.mPassword); */
	emit sendMessage(&mesg);
}

void MessageCodec::updateAppearance(UserData* userData) {
	c2sChangeAppearanceMessage mesg;
	mesg.mScaledPixmap = userData->scaledIcon().toImage();
	mesg.mUsernameBGColor = userData->usernameBGColor();
	mesg.mSpeechBalloonBGColor = userData->balloonBGColor();
	mesg.mUsernameFontColor = userData->usernameFontColor();
	mesg.mSpeechBalloonFontColor = userData->balloonFontColor();
	emit sendMessage(&mesg);
}


void MessageCodec::requestAppearance(const QUuid& guid, bool requestIcon, bool requestUsername, bool requestUsernameBGColor, bool requestUsernameFontColor, bool requestSpeechBalloonBGColor, bool requestSpeechBalloonFontColor) {		
	c2sRequestAppearanceMessage mesg;
	mesg.mGuid = guid;
	mesg.requestIcon = requestIcon;
	mesg.requestUsername = requestUsername;
	mesg.requestUsernameBGColor = requestUsernameBGColor;
	mesg.requestUsernameFontColor = requestUsernameFontColor;
	mesg.requestSpeechBalloonBGColor = requestSpeechBalloonBGColor;
	mesg.requestSpeechBalloonFontColor = requestSpeechBalloonFontColor;

	emit sendMessage(&mesg);
}

void MessageCodec::requestFollow(const QUuid& guid) {	
	c2sFollowUserMessage mesg;
	mesg.mFriendGuid = guid;
	emit sendMessage(&mesg);
}
void MessageCodec::requestUnfollow(const QUuid& guid) {
	c2sUnfollowUserMessage mesg;
	mesg.mFriendGuid = guid;
	emit sendMessage(&mesg);	
}

/*void MessageCodec::requestFriends(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "friends";
	emit sendMessage(&mesg);
}*/
void MessageCodec::requestFans(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "fans";
	emit sendMessage(&mesg);
}
void MessageCodec::requestFollowed(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "followed";
	emit sendMessage(&mesg);
}

void MessageCodec::requestStatus(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "status";
	emit sendMessage(&mesg);
}

void MessageCodec::requestProfile(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "profile";
	emit sendMessage(&mesg);
}

void MessageCodec::controller_serverKick(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mRequest = "kick";
	mesg.mGuid =guid;
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverRequestLocations() {
	UniversalRequestMessage mesg;
	mesg.mRequest = "locations";
	emit sendMessage(&mesg);
}

void MessageCodec::controller_serverUninviteFromExpedition(QList<UserData*> list) {
	c2sAskUnjoinMessage mesg;
	for(int i=0; i<list.count(); i++) {
		mesg.mGuids.append(list[i]->guid());	
	}
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverInviteToExpedition(const QColor& c, QList<UserData*> list) {
	QList<QUuid> guids;
	for(int i=0; i<list.count(); i++) {
		guids.append(list[i]->guid());	
	}
	c2sAskJoinMessage mesg;
	mesg.mGuids = guids;
	mesg.mColor = c;
	emit sendMessage(&mesg);
}

/*void MessageCodec::controller_serverBetaInvite(const QString& toEmailAddy, const QString& fromName) {
	c2sBetaInviteMessage mesg;
	mesg.mToEmail = toEmailAddy;
	mesg.mFromName = fromName;	
	emit sendMessage(&mesg);
}*/
/*void MessageCodec::controller_serverGetInviteCount() {
	UniversalRequestMessage mesg;
	mesg.mRequest="ic";
	emit sendMessage(&mesg);
}*/
void MessageCodec::controller_serverRequestPendingFollowers() {
	UniversalRequestMessage mesg;
	mesg.mRequest = "pf";
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverRequestPendingFollowing() {
	UniversalRequestMessage mesg;
	mesg.mRequest = "pfing";
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverAllowFollower(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mRequest = "allow_f";
	mesg.mGuid = guid;
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverDenyFollower(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mRequest = "deny_f";
	mesg.mGuid = guid;
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverBlock(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mRequest = "block";
	mesg.mGuid = guid;
	emit sendMessage(&mesg);
}

#ifdef ADMIN
void MessageCodec::controller_serverExecuteAdminCommand(const QString& username, int command, const QString& optionalArgument) {
	c2sAdminCommandMessage mesg;
	mesg.mUsername = username;
	mesg.mCommand = command;
	mesg.mOption = optionalArgument;
	emit sendMessage(&mesg);
}
#endif
void MessageCodec::controller_serverRequestPasswordReset() {
	c2sResetPasswordMessage mesg;
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverChangePassword(const QString& newPass) {
	c2sChangePasswordMessage mesg;
	//QCryptographicHash::hash(AuthenticationMessage::stPassword.toAscii(), QCryptographicHash::Sha1);
	mesg.mNewPass = QString(QCryptographicHash::hash(newPass.toAscii(), QCryptographicHash::Sha1));
	emit sendMessage(&mesg);
}
/*void MessageCodec::controller_serverConfirmKey(const QString& key) {
	c2sConfKeyMessage mesg;
	mesg.mKey = key;
	emit sendMessage(&mesg);
}*/	

void MessageCodec::controller_serverExpeditionRequestResponse(const QUuid& guid, bool response) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	if(response) {
		mesg.mRequest = "expedition_accept";
	} else {
		mesg.mRequest = "expedition_deny";
	}
	emit sendMessage(&mesg);
}

void MessageCodec::controller_serverRequestFavorites(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "favorites";
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverRequestHistory(const QUuid& guid) {
	UniversalRequestMessage mesg;
	mesg.mGuid = guid;
	mesg.mRequest = "history";
	emit sendMessage(&mesg);
}


void MessageCodec::controller_serverClearHistory() {
	UniversalRequestMessage mesg;
	mesg.mRequest = "cls_hist";
	emit sendMessage(&mesg);
}
void MessageCodec::controller_serverChangePersonalMotd(const QString& motd) {	
	c2sChangeMotdMessage mesg;
	mesg.mMessage = motd;
	emit sendMessage(&mesg);
}
void MessageCodec::changeProfile(const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo) {
	c2sChangeProfileMessage mesg;
	mesg.mOccupationInfo = occupationInfo;
	mesg.mHobbies = hobbies;
	mesg.mFavoriteCulture = favoriteCulture;
	mesg.mGeneralInfo = generalInfo;
	emit sendMessage(&mesg);
}

void MessageCodec::controller_addFavorite(const QString& name, const QUrl& url) {
	c2sAddBookmarksMessage mesg;
	mesg.mNames.append(name);
	mesg.mUrls.append(url);
	emit sendMessage(&mesg);
}
void MessageCodec::controller_removeFavorite(const QString& name, const QUrl& url) {
	c2sRemoveBookmarksMessage mesg;
	mesg.mNames.append(name);
	mesg.mUrls.append(url);
	emit sendMessage(&mesg);
}
void MessageCodec::controller_logHistory(const QString& name, const QUrl& url) {
	c2sAddHistoryMessage mesg;
	mesg.mNames.append(name);
	mesg.mUrls.append(url);
	emit sendMessage(&mesg);
}

