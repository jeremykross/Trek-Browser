#ifndef MESSAGECODEC_H
#define MESSAGECODEC_H

/* Basic */
#include <QObject>
#include <QUuid>
#include <QStringList>

/* Misc */
#include <QSettings>
#include <QCryptographicHash>

/* Standard */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "MessageBlocks.h"
#include "ServerConnection.h"
#include "UserData.h"
#include "UserDataFactory.h"
#include "Enumerations.h"

/* The MessageCodec is the class responsible for intercepting requests on the Controller, translating them into Messages, and sending them off to the ServerConnection.  Likewise, any Messages coming from the ServerConnection are interpreted here and fired off to the Controller as signals.  There's a sizable chunk of code here, unfortunately, given how simple most of this work is.  */

class MessageCodec : public QObject {
	Q_OBJECT
	public:	
		MessageCodec();
	public:
		void initialize();
		void registerServerConnection(ServerConnection* serverConnection);
	signals:
		void sendMessage(Message* mesg);
		void sendMessageEncrypted(Message* mesg);
		void communiqueReceived(const QUuid& guid,const QString& mesg);
		void personalCommuniqueReceived(const QUuid& guid, const QString& mesg);	
		void serverAuthenticationSuccess(UserData* userData);
		void serverAuthenticationFailure(const QString&);
		void addUser(UserData* userData);
		void removeUser(const QUuid& userGuid, const QUrl& url);
		void signupSuccess();
		void signupFailure(const QString& mesg);

		void alterAppearance(const QUuid& guid, const QPixmap& pixmap, const QString& username, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& ballonFontColor);
		void alterPartyAppearance(const QUuid& guid, const QColor& color, bool isLeader);
		void positionChanged(const QUuid& userGuid, unsigned int elementID, float percentX, float percentY);
		void positionChangedBackup(const QUuid& userGuid, float percentX, float percentY);

		void addFollowed(const QUuid& owner, QList<UserData*>friends);
		//void addFriends(const QUuid& owner, QList<UserData*>friends);
		void addFans(const QUuid& owner, QList<UserData*>fans);
		void controller_serverRemoveFans(const QUuid& owner, QList<UserData*>fans);
		void controller_serverRemoveFollowed(const QUuid& owner, QList<UserData*>followed);
		void updateStatus(const QUuid& guid, Status status, const QString& mesg,const QString& url);
		void updateProfile(const QUuid& guid,const QString& emailAddress, const QString& sex, const QDate& birthDate, const QString& mOccupationInfo, const QString& mHobbies, const QString& mFavoriteCulture, const QString& mGeneralInfo);
		void controller_favoritesDataReceived(const QUuid& guid, QList<SiteData*> data);
		void controller_historyDataReceived(const QUuid& guid, QList<SiteData*> data);

		void controller_askingExpeditionJoin(const QUuid& guid);

		void controller_partyMove(const QUrl& url); 
		void controller_partyAddUser(UserData* u);
		void controller_partyRemoveUser(const QUuid& guid);
		void controller_partyEjected();

		//void controller_inviteCountReceived(int count);

		void controller_pendingFollowersReceived(QList<UserData*> followers);
		void controller_pendingFollowingReceived(QList<UserData*> following);
		void controller_showSystemTrayMessage(const QString&, const QString&);
		//void controller_requestConfirmationKey();
		//void controller_confKeyValid(bool);

		void controller_updateMotd(const QString&);

	public slots:
		void receiveMessage(Message* mesg);
		void authenticate(const QString& username, const QString& password);
		void speak(const QString& communique);
		void personalSpeak(const QUuid& recipient, const QString& communique);
		void move(unsigned int id, int locationX, int locationY, int elementX, int elementY, int elementWidth, int elementHeight,int bodyWidth, int bodyHeight);	
		void changeLocale(QUrl oldUrl, QUrl newUrl);
		void signup(const QString& emailAddress, const QString& firstName, const QString& lastName, const QString& sex,const QDate& birthDate, const QString& desiredUsername, const QString& password, const QString& authKey);
		void updateAppearance(UserData* userData);
		void requestAppearance(const QUuid& guid, bool requestIcon, bool requestUsername, bool requestUsernameBGColor, bool requestUsernameFontColor, bool requestSpeechBalloonBGColor, bool requestSpeechBalloonFontColor);

		void requestFollow(const QUuid& guid);
		void requestUnfollow(const QUuid& guid);

		void requestFollowed(const QUuid& guid);
		void requestFans(const QUuid& guid);
	//	void requestFriends(const QUuid& guid);

		void requestStatus(const QUuid& guid);
		
		void requestProfile(const QUuid& guid);

		void controller_serverKick(const QUuid& guid);

		void controller_serverRequestLocations();

		void controller_serverRequestFavorites(const QUuid& guid);
		void controller_serverRequestHistory(const QUuid& guid);
		
		void controller_serverChangePersonalMotd(const QString&);
		void changeProfile(const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo);

		void controller_serverClearHistory();


		void controller_addFavorite(const QString& name, const QUrl& url);
		void controller_removeFavorite(const QString& name, const QUrl& url);
		void controller_logHistory(const QString& name, const QUrl& url);

		void controller_serverInviteToExpedition(const QColor&, QList<UserData*> list);
		void controller_serverUninviteFromExpedition(QList<UserData*> list);

		void controller_serverExpeditionRequestResponse(const QUuid& guid, bool);
		//void controller_serverBetaInvite(const QString& toEmailAddy, const QString& fromName);
		//void controller_serverGetInviteCount();

		void controller_serverRequestPendingFollowers();
		void controller_serverRequestPendingFollowing();
		void controller_serverAllowFollower(const QUuid& guid);
		void controller_serverDenyFollower(const QUuid& guid);
		void controller_serverBlock(const QUuid& guid);

		//void controller_serverConfirmKey(const QString&);

		void controller_serverRequestPasswordReset();
		void controller_serverChangePassword(const QString& newPass);
#ifdef ADMIN
		void controller_serverExecuteAdminCommand(const QString& username, int command, const QString& optionalArgument);
#endif

	//	void controller_serverRevokeFollowRequest(const QUuid& guid);

	protected:
		void processAuthenticated(s2cAuthenticatedMessage* mesg);
		void processAddUsers(s2cAddUsersMessage* mesg);
		void processRemoveUsers(s2cRemoveUsersMessage* mesg);
		void processAddFollowed(s2cAddFollowedMessage* mesg);
		void processAddFans(s2cAddFansMessage* mesg);
		void processRemoveFans(s2cRemoveFansMessage* mesg);
		void processRemoveFollowed(s2cRemoveFollowedMessage* mesg);
		void processListen(s2cListenMessage* mesg);
		void processPersonalListen(s2cPersonalListenMessage* mesg);
		void processSignupResponse(s2cSignupResponseMessage* mesg);
		void processAppearance(AppearanceMessage* mesg);
		void processMove(s2cMoveMessage* mesg);
		void processStatus(s2cStatusMessage* mesg);
		void processProfile(s2cProfileMessage* mesg);
		void processAddBookmarks(s2cAddBookmarksMessage* mesg);
		void processAddHistory(s2cAddHistoryMessage* mesg);
		void processPartyMove(s2cPartyMoveMessage* mesg);
		void processPartyAddUsers(s2cPartyAddUsersMessage* mesg);
		void processPartyRemoveUsers(s2cPartyRemoveUsersMessage* mesg);
		//void processInviteCount(s2cInviteCountMessage* mesg);
		void processSay(s2cSayMessage* mesg);
		void processUniversalRequest(UniversalRequestMessage* mesg);

		void processNonce(s2cNonceMessage* mesg);
		void processPendingFollowers(s2cPendingFollowersMessage* mesg);
		void processPendingFollowing(s2cPendingFollowingMessage* mesg);
};

#endif
