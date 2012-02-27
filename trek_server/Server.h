#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTimer>
#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QUrl>
#include <QUuid>
#include <QString>
#include <QRegExp>
#include <QMap>
#include <QList>
#include <QVariant>
#include <QThread>
#include <QImage>

#include <iostream>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QCryptographicHash>

#include <QStringList>
#include <QProcess>

#include <QtConcurrentRun>

#include <QDate>
#include <QSettings>

#include "ClientData.h"
#include "MessageBlocks.h"

#define CONNECT_TO_DB      if(connectToDB() == false) { std::cout << "DB Error\n"; return; }
#define CONNECT_TO_DB_VAL  if(connectToDB() == false) {std::cout << "DB Error\n"; return -1; }

#define ADMIN


class Server: public QTcpServer {
	Q_OBJECT;
	public:
		Server(QObject* parent=0);
	protected slots:
		void processClientMessage(ClientData* client, Message* mesg);
		void clientDisconnected(ClientData* client);
	protected:
		QString helperMakeNonce(int nonceSize=5);
		bool connectToDB();
		void incomingConnection(int socketId);	
		void processUniversalRequest(ClientData* client, UniversalRequestMessage* mesg);
		//void processClientBetaInvite(ClientData* client, c2sBetaInviteMessage* mesg);
		//void processConfKey(ClientData* client, c2sConfKeyMessage* mesg);
		void processChangeMotd(ClientData* client, c2sChangeMotdMessage* mesg);
		void processClientAuthentication(ClientData* client, AuthenticationMessage* mesg);
		void processClientLocaleChanged(ClientData* client, c2sLocaleChangedMessage* mesg);
		void helperAllowLogin(ClientData* client);
		void processClientSpeak(ClientData* client, c2sSpeakMessage* mesg);
		void processClientPersonalSpeak(ClientData* client, c2sPersonalSpeakMessage* mesg);
		void processClientSignup(ClientData* client, c2sSignupMessage* mesg);
		void processClientChangeAppearance(ClientData* client, c2sChangeAppearanceMessage* mesg);
		void processClientRequestAppearance(ClientData* client, c2sRequestAppearanceMessage* mesg);
		void processClientMove(ClientData* client, c2sMoveMessage* mesg);
		void processClientFollowUser(ClientData* client, c2sFollowUserMessage* mesg);
		void processClientUnfollowUser(ClientData* client, c2sUnfollowUserMessage* mesg);
		void processClientChangeProfile(ClientData* client, c2sChangeProfileMessage* mesg);
		unsigned int helperGetOrAddPage(const QString& name, const QString& url);
		void processClientAddBookmarks(ClientData* client, c2sAddBookmarksMessage* mesg);
		void processClientRemoveBookmarks(ClientData* client, c2sRemoveBookmarksMessage* mesg);
		void processClientAddHistory(ClientData* client, c2sAddHistoryMessage* mesg);
		void processClientAskJoin(ClientData* client, c2sAskJoinMessage* mesg);
		void processClientAskUnjoin(ClientData* client, c2sAskUnjoinMessage* mesg);
		void processClientResetPassword(ClientData* client, c2sResetPasswordMessage* mesg);
		void processClientChangePassword(ClientData* client, c2sChangePasswordMessage* mesg);
#ifdef ADMIN
		void processClientAdminCommand(ClientData* client, c2sAdminCommandMessage* mesg);
#endif

		void informRelationsOfStatusChange(const QUuid& guid, Status s);

		void informPartyOfArrival(ClientData* client); 
		void informPartyOfDeparture(ClientData* client);

	//	QList<unsigned int> getFollowed(const QUuid& guid);
	//	QList<unsigned int> getFans(const QUuid& guid);

		void sendToUrl(const QUrl& url, Message* mesg);

		void helperChangeRelationship(const QUuid& follower, const QUuid& followee, const QString& status);
		int helperIdForUsername(const QString& username);
		int helperIdForGuid(const QUuid& guid);
		void helperSendTextMessage(ClientData* client, const QString& mesg);
		//void helperResetEmail(const QString& emailAddy);
		void helperSendEmail(const QString& emailAddy, const QString& subject, const QString& message);
		QString helperResetPassword(ClientData* client);

		//void processClientSpeak
		QMap<QUuid, ClientData*> mDataByGuid;
		QMap<unsigned int, ClientData*> mDataById;
		QMap<QUrl, QList<ClientData*> > mDataByUrl;

		QMap<ClientData*, QMap<ClientData*, bool> > mExpeditionMap;
};

#endif
