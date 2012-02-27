#ifndef USERDATAFACTORY_H
#define USERDATAFACTORY_H

#include <QObject>
#include <QMap>
#include <QUuid>
#include <QPixmap>
#include <QString>
#include <QLabel>

#include <iostream>

#include "Controller.h"
#include "UserData.h"

class UserDataFactory: public QObject {
	Q_OBJECT;
	public:
		~UserDataFactory();
		static UserDataFactory* self();
		UserData* userDataForGuid(const QUuid& guid);
		bool isUserDataKnown(const QUuid& guid);
		//void requestAppearance(const QUuid& guid);
		//void requestStatus(const QUuid& guid);
	signals:
		void controller_serverRequestAppearance(const QUuid& guid, bool requestIcon, bool requestUsername, bool requestUsernameBGColor, bool requestUsernameFontColor, bool requestBalloonBGColor, bool requestBalloonFontcolor);
		void controller_serverRequestStatus(const QUuid& guid);
		void controller_userOnline(UserData* userData);
		void controller_userOffline(UserData* userData);
	protected slots:
		void controller_avatarAppearanceChanged(const QUuid& guid, const QPixmap& pixmap, const QString& username, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& balloonFontColor);
		void controller_statusUpdated(const QUuid& guid, Status status, const QString& mesg, const QString& url);


	private:
		UserDataFactory(); 
		static UserDataFactory* stInstance;

		QMap<QUuid, UserData*> mKnownUserData;
};

#endif
