#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QAbstractSocket>
#include <QTcpSocket>
#include "MessageBlocks.h"

class Connection : public QObject {
	Q_OBJECT
	public:
		Connection(QObject* parent=NULL);
		~Connection();
		QTcpSocket* socket();
	signals:
		void messageReceived(Connection* me, Message* message);
		void disconnected(Connection* me);
	public slots:
		void sendMessage(Message* mesg);
	protected slots:
		virtual void socketStateChanged(QAbstractSocket::SocketState state) {}
		virtual void socketConnected() {}
		virtual void socketDisconnected() {}
		void readSocket();
		virtual void processMessage(Message* mesg) {}
	protected:
		QTcpSocket mSocket;
		quint16 mNextBlockSize;
};

#endif
