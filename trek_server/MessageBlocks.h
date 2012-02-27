#ifndef MESSAGEBLOCKS_H
#define MESSAGEBLOCKS_H

#include <QIODevice>
#include <QDataStream>
#include <QUuid>
#include <QString>
#include <QPixmap>
#include <QColor>
#include <QUrl>
#include <QDate>

#include <iostream>

#include "Enumerations.h"

enum MessageType {
	NO_OP,	

	AUTHENTICATION,
	s2cAUTHENTICATED,
	c2sSPEAK,
	s2cLISTEN,

	c2sPERSONAL_SPEAK,
	s2cPERSONAL_LISTEN,

	c2sMOVE,
	s2cMOVE,

	s2cSAY,

	c2sLOCALE_CHANGED,
	s2cADD_USERS,
	s2cREMOVE_USERS,
		
	c2sCHANGE_APPEARANCE,
	c2sREQUEST_APPEARANCE,	
	APPEARANCE,
	
	s2cPROFILE,
	c2sCHANGE_PROFILE,
	c2sCHANGE_MOTD,
		
	c2sSIGNUP,
	s2cSIGNUP_RESPONSE,
//	s2cSIGNUP_SUCCESS,
//	s2cSIGNUP_FAILURE,

	s2cADD_FOLLOWED,
	s2cADD_FRIENDS,
	s2cADD_FANS,
	s2cREMOVE_FANS,
	s2cREMOVE_FOLLOWED,
	c2sFOLLOW_USER,
	c2sUNFOLLOW_USER,
	s2cSTATUS,
	c2sADD_BOOKMARKS,
	c2sREMOVE_BOOKMARKS,
	s2cADD_BOOKMARKS,
	c2sADD_HISTORY,
	c2sREMOVE_HISTORY,
	s2cADD_HISTORY,

	c2sASK_JOIN,
	c2sASK_UNJOIN,
	s2cPARTY_ADD,
	s2cPARTY_REMOVE,
	s2cPARTY_MOVE,

	s2cNONCE,
	c2sBETA_INVITE,
	s2cINVITE_COUNT,
	s2cPENDING_FOLLOWERS,
	s2cPENDING_FOLLOWING,
	c2sCONF_KEY,
	c2sRESET_PASSWORD,
	c2sCHANGE_PASSWORD,
	c2sADMIN_COMMAND,
	UNIVERSAL_REQUEST,
	MESSAGE_TYPE_1_LAST
};


class Message {
	public:
		Message();
		virtual  ~Message() {}
		bool enoughDataPresent(QIODevice* device);
		//readEcnrypted() writeEncrypted()
		bool read(QIODevice* device, int position=0);
		bool write(QIODevice* device,int position=0, bool prependStats=true);
		virtual void print();
		MessageType mType;
		quint32 blockSize;
	protected:
		virtual void derivedRead(QDataStream& in){std::cout << "called read on base\n";}
		virtual void derivedWrite(QDataStream& out){std::cout << "called write on base\n";}
};

class s2cNonceMessage: public Message {
	public:
		s2cNonceMessage();
		QString mNonce;
		void print();
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);

};

class AuthenticationMessage: public Message {
	public:
		AuthenticationMessage();
		void print();
	//	bool read(QIODevice* device, int position=0);
	//	bool write(QIODevice* device,int position=0, bool prependStats=true);
		QString mUsername;
		QString mPassword;
		static QString stUsername;
		static QString stPassword;
		static QString stNonce;
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};

class c2sBetaInviteMessage: public Message {
	public:
		c2sBetaInviteMessage();
		QString mToEmail;
		QString mFromName;
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};

class s2cInviteCountMessage: public Message {
	public:
		s2cInviteCountMessage();
		int mCount;	
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};

class s2cAuthenticatedMessage:public Message {
	public:
		s2cAuthenticatedMessage();
		void print();
		bool mIsSuccessful;
		bool mPWIncorrect;
		QUuid mGuid;
		QString mUsername;
		
		QString mCause;

	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};

class c2sSpeakMessage:public Message {
	public:
		c2sSpeakMessage();
		void print();
		QString mCommunique;
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};

class c2sPersonalSpeakMessage:public Message {
	public:
		c2sPersonalSpeakMessage();
		QUuid mGuid;
		QString mCommunique;
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};
class s2cPersonalListenMessage:public c2sPersonalSpeakMessage {
	public:
		s2cPersonalListenMessage();
};

class s2cListenMessage:public Message {
	public:	
		s2cListenMessage();
		void print();
		QUuid mGuid;
		QString mCommunique;	
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};

class AppearanceMessage : public Message {
	public:
		AppearanceMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		bool hasIcon;
		bool hasUsername;
		bool hasUsernameBGColor;
		bool hasSpeechBalloonBGColor;
		bool hasUsernameFontColor;
		bool hasSpeechBalloonFontColor;
		bool hasPartyColor;
		QUuid mGuid;
		QImage mIcon;
		QString mUsername;
		QColor mUsernameBGColor;
		QColor mSpeechBalloonBGColor;
		QColor mUsernameFontColor;
		QColor mSpeechBalloonFontColor;
		QColor mPartyColor;
		bool mIsLeader;
};

class c2sRequestAppearanceMessage:public Message {
	public:
		c2sRequestAppearanceMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		bool requestIcon;
		bool requestUsername;
		bool requestUsernameBGColor;
		bool requestSpeechBalloonBGColor;
		bool requestUsernameFontColor;
		bool requestSpeechBalloonFontColor;
		QUuid mGuid;
};

class c2sLocaleChangedMessage : public Message {
	public:
		c2sLocaleChangedMessage();
		void print();
		void derivedRead(QDataStream& in);	
		void derivedWrite(QDataStream& out);
		QUrl mNewUrl;
		QUrl mOldUrl;
};

class s2cAddUsersMessage : public Message {
	public:
		s2cAddUsersMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		quint16 mUsersCount;
		QList<QUuid> mGuids;
		QList<QString> mUsernames;
};
class s2cAddRelatedUsersMessage: public s2cAddUsersMessage {
	public:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QUuid mOwnerGuid;
};

class s2cAddFollowedMessage: public s2cAddRelatedUsersMessage {
	public:
		s2cAddFollowedMessage();
};

class s2cAddFriendsMessage:public s2cAddRelatedUsersMessage {
	public:
		s2cAddFriendsMessage();
};
class s2cAddFansMessage:public s2cAddRelatedUsersMessage {
	public:
		s2cAddFansMessage();
};
class s2cRemoveFansMessage:public s2cAddFansMessage {
	public:
		s2cRemoveFansMessage();
};
class s2cRemoveFollowedMessage:public s2cAddFollowedMessage {
	public: 
		s2cRemoveFollowedMessage();
};
class s2cPendingFollowersMessage:public s2cAddUsersMessage {
	public:
		s2cPendingFollowersMessage();
};
class s2cPendingFollowingMessage:public s2cAddUsersMessage {
	public:
		s2cPendingFollowingMessage();
};

class s2cRemoveUsersMessage: public Message {
	public:
		s2cRemoveUsersMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		quint16 mUsersCount;
		QList<QUuid> mGuids;
		QList<QUrl> mUrls;
};

class c2sRequestFriendsMessage: public Message {
	public: 
		c2sRequestFriendsMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QUuid mGuid;
};
class c2sFollowUserMessage: public Message {
	public:
		c2sFollowUserMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QUuid mFriendGuid;
};
class c2sUnfollowUserMessage:public c2sFollowUserMessage {
	public:
		c2sUnfollowUserMessage();
};

class SiteMessage: public Message {
	protected:
		SiteMessage();
	public:
		QList<QString> mNames;
		QList<QUrl> mUrls;	
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};
class c2sAddBookmarksMessage:public SiteMessage {
	public:
		c2sAddBookmarksMessage();
};
class c2sRemoveBookmarksMessage:public SiteMessage{
	public:
		c2sRemoveBookmarksMessage();
};
class s2cAddBookmarksMessage:public SiteMessage {
	public:		
		s2cAddBookmarksMessage();	
		QUuid mGuid;
	protected:
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
};
/* end bookmarks */
/* start history */
class c2sAddHistoryMessage:public SiteMessage {
	public:
		c2sAddHistoryMessage() { mType = c2sADD_HISTORY;}
};
class c2sRemoveHistoryMessage:public SiteMessage {
	public:
		c2sRemoveHistoryMessage() { mType = c2sREMOVE_HISTORY;}
};

class s2cAddHistoryMessage:public s2cAddBookmarksMessage {
	public:	
		s2cAddHistoryMessage() { mType = s2cADD_HISTORY; }
};

class s2cPartyAddUsersMessage: public s2cAddUsersMessage {
	public:
		s2cPartyAddUsersMessage();	
};
class s2cPartyRemoveUsersMessage: public s2cRemoveUsersMessage {
	public:
	s2cPartyRemoveUsersMessage();	
};
class s2cPartyMoveMessage: public Message {
	public:
		s2cPartyMoveMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);	
		QUrl mUrl;
};

/*------------------------------------*/

class UniversalRequestMessage: public Message {
	public:
		UniversalRequestMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QUuid mGuid;
		QString mRequest;
};

class c2sSignupMessage: public Message {
	public:
		c2sSignupMessage();	
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QString mEmailAddress;
		QString mFirstName;
		QString mLastName;
		QString mSex;
		QDate mBirthDate;
		QString mDesiredUsername;
		QString mPassword;
		QString mKey;
};
class s2cSignupResponseMessage: public Message {
	public:
		s2cSignupResponseMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		bool mSuccessful;
		QString mMesg;
};

class s2cProfileMessage: public Message {
	public:
		s2cProfileMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);

		QUuid mGuid;
		QString mEmailAddress;
		QString mGender;
		QDate mBirthDate;
		QString mOccupationInfo;
		QString mHobbies;
		QString mFavoriteCulture;
		QString mGeneralInfo;
		QString mMotd;
};

class c2sChangeMotdMessage: public Message {
	public:
		c2sChangeMotdMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);

		QString mMessage;
};
class c2sChangeProfileMessage: public Message {
	public:
		c2sChangeProfileMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);

		QString mOccupationInfo;
		QString mHobbies;
		QString mFavoriteCulture;
		QString mGeneralInfo;
};

class c2sChangeAppearanceMessage: public Message {
	public:
		c2sChangeAppearanceMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QImage mScaledPixmap;
		QColor mUsernameBGColor;
		QColor mSpeechBalloonBGColor;
		QColor mUsernameFontColor;
		QColor mSpeechBalloonFontColor;
};

class MoveMessage: public Message {
	public:
		MoveMessage();
		void print();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);

		unsigned int mId;
		int mLocationX;
		int mLocationY;
		int mElementX;
		int mElementY;
		int mElementWidth;
		int mElementHeight;
		int mBodyWidth;
		int mBodyHeight;
			
};

class c2sMoveMessage:public MoveMessage {
	public:
		c2sMoveMessage();	
};

class s2cMoveMessage:public MoveMessage {
	public:
		s2cMoveMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QUuid mGuid;
};
class s2cSayMessage:public Message {
	public:
		s2cSayMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QString mMessage;
};

class s2cStatusMessage:public Message {
	public:
		s2cStatusMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QUuid mGuid;
		int mStatus;
		QString mUrl;
		QString mMessage;
};

#ifdef ADMIN
class c2sAdminCommandMessage: public Message {
	public:
		enum AdminCommand {
			SAY,
			MOTD,
			KICK,
			BLOCK,
			SPEAK,
			MOVE_TO
		};
		c2sAdminCommandMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QString mUsername;
		unsigned char mCommand;	
		QString mOption;
};
#endif

class c2sChangePasswordMessage: public Message {
	public:
		c2sChangePasswordMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QString mNewPass;
};
class c2sResetPasswordMessage:public Message {
	public:
		c2sResetPasswordMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QString mEmailAddress;
};
class c2sConfKeyMessage:public Message {
	public:
		c2sConfKeyMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QString mKey;
};
class c2sAskJoinMessage:public Message {
	public:
		c2sAskJoinMessage();
		void derivedRead(QDataStream& in);
		void derivedWrite(QDataStream& out);
		QList<QUuid> mGuids;
		QColor mColor;
};

class c2sAskUnjoinMessage:public c2sAskJoinMessage {
	public:
		c2sAskUnjoinMessage();
};

/*class c2sMoveMessage: public Message {
  public:
  c2sMoveMessage();
  void print();
  void derivedRead(QDataStream& in);
  void derivedWrite(QDataStream& out);

  };*/

class MessageFactory {
	public:
		static Message* getMessageByType(unsigned short type); 
};
#endif
