#ifndef CLIENTDATA_H
#define CLIENTDATA_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QSslSocket>
#include <QThread>
#include <QDataStream>
#include <QString>
#include <QUuid>
#include <QUrl>

#include <iostream>

#include "MessageBlocks.h"

//Rename to ClientConnection
class ClientData: public QObject {
	Q_OBJECT
	public:
		ClientData(QObject* parent=NULL);
		~ClientData();
		QTcpSocket* socket();
		QUrl currentUrl();
		QUuid guid();
		unsigned int id();
		QString username();
		QString emailAddress();
		bool isMoved();
		s2cMoveMessage* mostRecentMove();
		void setGuid(QUuid guid);
		void setId(unsigned int id);
		void setUsername(const QString& username);
		void setEmailAddress(const QString& emailAddy);
		void setUrl(const QUrl& url);
		void setMostRecentMove(s2cMoveMessage mesg);
		void setLeader(ClientData* leader);
		void setNonce(const QString& nonce);
		QString nonce();
		ClientData* leader();
		void setPartyColor(const QColor& c);
		QColor partyColor();
	signals:
		void messageReceived(ClientData* client, Message* message);
		void disconnected(ClientData* client);
	public slots:
		void sendMessage(Message* mesg);
		void disconnect();
	protected slots:
		void socketStateChanged(QAbstractSocket::SocketState state);
		void socketConnected();
		void socketDisconnected();
		void readSocket();
		bool processMessage(Message* mesg); 
	protected:
		ClientData* mCurrentLeader;

		QSslSocket mSocket;
		QString mUsername;
		QString mEmailAddress;
		QUuid mGuid;
		unsigned int mId;
		QUrl mUrl;
		quint16 mNextBlockSize;

		s2cMoveMessage mMostRecentMove;
		QColor mPartyColor;

		QString mNonce;
};

#endif
