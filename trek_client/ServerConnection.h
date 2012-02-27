#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

/* Networking */
#include <QTcpSocket>
#include <QSslSocket>
#include <QAbstractSocket>

/* Data */ 
#include <QIODevice>
#include <QDataStream>
#include <QByteArray>
#include <QHostAddress>
#include <QBuffer>

/* Basic */
#include <QString>
#include <QObject>

/* Gui */
#include <QMessageBox>
#include <QApplication>

/* Std */
#include <iostream>

#include "Controller.h"
#include "MessageBlocks.h"


/* The ServerConnection represents the link between this Trek client and the Trek server.  All server communication is encapsulated in this class (simply needs instantiated from main(). */

class ServerConnection: public QObject {
	Q_OBJECT
	public:
		ServerConnection();
		~ServerConnection();
	signals:
		void failedToConnect();
		void alreadyConnected();
		void connectionLost();
		void messageReceived(Message* mesg);
	public slots:
		void initialize();
		void connect();
		void disconnect();
		void sendMessage(Message* mesg);
		void readSocket(); 
		bool processMessage(Message* mesg);
	protected slots:
		void error();
		void connected();
		void disconnected();
	protected:
		QTcpSocket mSocket;
};

#endif
