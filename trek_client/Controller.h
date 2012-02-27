#ifndef CONTROLLER_H
#define CONTROLLER_H

#define ICON_SIZE 256

#include <QObject>
#include <QUuid>
#include <QUrl>
#include <QString>
#include <QPixmap>
#include <iostream>
#include <QNetworkRequest>
#include <QWebPage>
#include <QDate>

#include "GraphicsAvatarItem.h"
#include "GraphicsSpeechBalloonItem.h"
#include "UserData.h"
#include "MessageBlocks.h"
#include "DownloadData.h"
#include "SignupDataWidget.h"
#include "SiteListItem.h"
#include "SiteListItemModel.h"
#include "DownloadModel.h"

class RelationshipListModel;

class Controller: public QObject {
	Q_OBJECT;
	public:
		static Controller* self();
		GraphicsAvatarItem* constructUserAvatar();
		void helperAddAvatar(UserData* userData);
	
	signals:	
		void avatarAdded(UserData* userData); 	
		void avatarRemoved(UserData* userData);
		void avatarRemoved(const QUuid& guid, const QUrl& newUrl);
		void avatarAppearanceChanged(const QUuid& guid, const QPixmap& pixmap, const QString& username, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& balloonFontColor);
		void avatarPartyAppearanceChanged(const QUuid& guid, const QColor& color, bool isLeader);

		void communiqueReceived(const QUuid& guid, const QString& mesg);	
		void personalCommuniqueReceived(const QUuid& guid, const QString& mesg);
		void avatarPositionChanged(const QUuid& guid, unsigned int elementId, float percentX, float percentY);
		void avatarPositionChangedBackup(const QUuid& guid, float percentX, float percentY);

		void serverRequestAppearance(const QUuid& guid, bool requestIcon, bool requestUsername, bool requestUsernameBGColor, bool requestUsernameFontColor, bool requestSpeechBalloonBGColor, bool requestSpeechBalloonFontColor);

	//	void serverUpdateAppearance(const QPixmap& pixmap, const QString& username, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& balloonFontColor);
		void serverUpdateAppearance(UserData* userData);

		void serverRequestProfile(const QUuid& guid);
		void serverUpdateProfile(const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo);
		void serverChangePersonalMotd(const QString& newMotd);
		void updateMotd(const QString& motd);

		void serverClearHistory();
			
		void serverConnectionAttempted();
		void serverDisconnect();
		void serverConnectionLost();

		void serverSignupAttempted(const QString& emailAddress, const QString& firstName, const QString& lastName, const QString& sex, const QDate& birthDate, const QString& desiredUsername, const QString& passWord, const QString& authKey);

		void serverSignupFailure(const QString& mesg);
		void serverSignupSuccess();

		void serverAuthenticationAttempted(const QString& username, const QString& password);
		void serverSpeak(const QString& communique);
		void serverChangeLocale(QUrl oldUrl, QUrl newUrl);
		void serverChangePosition(unsigned int id, int locationX, int locationY, int elementX, int elementY, int elementWidth, int elementHeight,int bodyWidth, int bodyHeight);

		void serverPersonalSpeak(const QUuid& recipient, const QString& communique);
		
		void serverConnectionFailure();
		void serverConnected();
		void serverAuthenticationSuccess(UserData* userData);
		void serverAuthenticationFailure(const QString& cause);
		void serverDisconnected();
		void serverRegistrationAttempted();
		void serverRequestFollow(const QUuid& guid);
		void serverRequestUnfollow(const QUuid& guid);

		void serverRequestFollowed(const QUuid& guid);
		//void serverRequestFriends(const QUuid& guid);
		void serverRequestFans(const QUuid& guid);

		void serverInviteToExpedition(const QColor&, QList<UserData*> list);
		void serverUninviteFromExpedition(QList<UserData*> list);

		void serverExpeditionRequestResponse(const QUuid& guid, bool repsonse);

                void serverRequestPendingFollowers();
		void serverRequestPendingFollowing();
		void pendingFollowersReceived(QList<UserData*> followers);
		void pendingFollowingReceived(QList<UserData*> followers);

		void serverAllowFollower(const QUuid& guid);
		void serverDenyFollower(const QUuid& guid);
		void serverBlock(const QUuid& guid);
		void serverRevokeFollowRequest(const QUuid& guid);


/*------------------*/
		void serverRequestFavorites(const QUuid& guid);
		void favoritesDataReceived(const QUuid& guid, QList<SiteData*> data); 
		void favoritesModelRequested(const QUuid& guid);

		void favoritesModelReceived(const QUuid& guid, SiteListItemModel* model);
/*-------------------*/
		void serverRequestHistory(const QUuid& guid);
		void historyDataReceived(const QUuid& guid, QList<SiteData*> data);
		void historyModelRequested(const QUuid& guid);
		void historyModelReceived(const QUuid& guid, SiteListItemModel* model);
/*---------------------*/
		void downloadModelRequested();
		void downloadModelReceived(DownloadModel* model);


		void serverRequestStatus(const QUuid& guid);

		void serverKick(const QUuid& guid);
		void serverRequestLocations();

		void serverExecuteAdminCommand(const QString& username, int adminCommand, const QString& optionalArgument);

		void serverRequestPasswordReset(const QString& emailAddy);
		void serverRequestPasswordReset();
		void serverChangePassword(const QString&);

		void downloadRequested(const QNetworkRequest& request);
		void downloadStarted(DownloadData* download);

		void bookmarksRequested();
		void bookmarkModelRequested();
		void bookmarksReceived(SiteListItem* root);
		void bookmarkModelReceived(SiteListItemModel* model);
		void heartSetRequested(const QUrl&);
		void heartSetReceived(const QUrl&, bool on);

		void historyRequested();
		void historyModelRequested();
		void historyReceived(SiteListItem* root);
		void historyModelReceived(SiteListItemModel* model);

		void logHistory(const QString& name, const QUrl& url);
		void logPixmap(const QPixmap& pixmap);
		
		void addFavoriteFolder(const QString& name);
		void addFavorite(const QString& name, const QUrl& url);
		void removeFavorite(const QString& name, const QUrl& url);

		void followedReceived(const QUuid& owner, QList<UserData*> friends); //QList<QUuid> guids, QList<QString> usernames); 
		void fansReceived(const QUuid& owner, QList<UserData*> fans);
		void friendsReceived(const QUuid& owner, QList<UserData*> friends);
		void serverRemoveFans(const QUuid& owner, QList<UserData*> fans);
		void serverRemoveFollowed(const QUuid& owner, QList<UserData*> followed);

	//	void followedModelRequested();
	//	void followedModelReceived(FriendModel* model);

		void followedModelRequested(const QUuid&);
		void followedModelOffered(const QUuid& owner, RelationshipListModel* model);

		void fansModelRequested(const QUuid&);
		void fansModelOffered(const QUuid& owner, RelationshipListModel* model);

		void statusUpdated(const QUuid& guid, Status status, const QString& message, const QString& url);
		void profileUpdated(const QUuid& guid,const QString& emailAddress, const QString& sex, const QDate& birthDate, const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo);

		void askingExpeditionJoin(const QUuid& guid);
		void partyMove(const QUrl& url);
		void partyAddUser(UserData* userData);
		void partyRemoveUser(const QUuid&);
		void partyEjected();

		//void addBookmark(const QUrl& url, const QString& name="",const QString& folderPath="/";
		//void removeBookmark(const QUrl& url, const QString& folderPath);
		//void moveBookmark(const QUrl& url, const QString& oldFolderPath, const QString& newFolderPath);
		//void bookmarksReady(SiteListFolder* rootFolder);
		//void requestBookmarks();
		//void receiveBookmarks(SiteListFolder* rootFolder);

		void userContextMenuRequested(UserData* data);
		void userDoubleClicked(UserData* data);
		void linkActivated(const QUrl& link);
		void linkActivated(const QString& link);

		void userOnline(UserData* userData);
		void userOffline(UserData* userData);

		//void serverBetaInvite(const QString& emailAddy, const QString& from);
		//void serverGetInviteCount();
		//void inviteCountReceived(int count);

		//void requestConfirmationKey();
		//void serverConfirmKey(const QString& key);
		//void confKeyValid(bool isValid);

		void showSystemTrayMessage(const QString& title, const QString& message);
	
	private:
		Controller() {}
		static Controller* mInstance;	
};

#endif
