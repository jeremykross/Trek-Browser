#ifndef DATASERVER_H
#define DATASERVER_H

#include <QTcpServer>
#include "Connection.h"
#include "MessageBlocks.h"

class DataServer: public QTcpServer {
	Q_OBJECT
	public:
		DataServer(QObject* parent=NULL);
	protected slots:
		void processMessage(Connection* connection, Message* message);
		void disconnected(Connection* connection);
	protected:
		void incomingConnection(int socketId);
};

#endif
