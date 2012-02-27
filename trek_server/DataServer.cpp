#include "DataServer.h"

DataServer::DataServer(QObject* parent) {}

void DataServer::incomingConnection(int socketId) {
	Connection* connection = new Connection();
	connection->socket()->setSocketDescriptor(socketId);
	connect(connection, SIGNAL(messageReceived(Connection*, Message*)), this, SLOT(processMessage(Connection*, Message*)));
	connect(connection, SIGNAL(disconnected(Connection*)), this, SLOT(disconnected(Connection*)));	
}

void DataServer::processMessage(Connection* connection, Message* message) {
}

void DataServer::disconnected(Connection* connection) {
	connection->deleteLater();
}
