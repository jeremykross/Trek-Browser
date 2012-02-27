#ifndef USERDATA_H
#define USERDATA_H

#include <QObject>
#include <QPixmap>
#include <QString>
#include <QFont>
#include <QColor>
#include <QUuid>
#include <QVariant>

#include <iostream>

#include "AvatarDefines.h"
#include "Enumerations.h"

#define USERDATA_TYPE QVariant::UserType + 10

#define MAC_OS

class UserData : public QObject {
	Q_OBJECT;
	public :
		UserData();
		QPixmap icon();
		void setIcon(QPixmap icon);
		bool hasIcon();
		QPixmap scaledIcon();
		QPixmap bigIcon();
		QString displayName();
		void setDisplayName(QString displayName);
		QFont font();
		void setFont(QFont font);
		
		QColor balloonBGColor();
		void setBalloonBGColor(QColor color);
		QColor usernameBGColor();
		void setUsernameBGColor(QColor color);
		QColor balloonFontColor();
		void setBalloonFontColor(QColor color);
		QColor usernameFontColor();
		void setUsernameFontColor(QColor color);
		void setGuid(QUuid guid);
		QUuid guid();

		void setPartyColor(const QColor& c, bool isLeader);
		QColor partyColor();
		bool isLeader();

		void setStatus(Status s);
		Status status();
		QString statusAsString();
		void setPersonalMessage(const QString& mesg);
		QString personalMessage();
		void setLocaleUrl(const QString& localeUrl);
		QString localeUrlAsString();

		void altered();
		void informStatusAltered();
	
		static void initializeDefaultValues();
		static QMap<QString, QVariant> stDefaultValues;
		static bool stDefaultsInitialized;

		//bool operator<(const UserData* other);
	signals:
		void dataAltered();
		void statusAltered();
	protected:
		QUuid mGuid;

		QPixmap mIcon;
		QPixmap mScaledIcon;
		QString mDisplayName;
		QFont   mFont;
		QColor  mUsernameBGColor;
		QColor  mSpeechBalloonBGColor;
		QColor  mUsernameFontColor;
		QColor  mSpeechBalloonFontColor;
		QColor  mPartyColor;
		bool    mIsLeader;

		Status mStatus;
		QString mMessage;
		QString mUrl;

		bool mHasIcon;
};

Q_DECLARE_METATYPE(UserData*);
#endif
