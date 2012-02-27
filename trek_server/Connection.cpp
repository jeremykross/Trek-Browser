#include "Connection.h"

Connection::Connection(QObject* parent) {
	connect(&mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(readSocket()));
	connect(&mSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
	connect(&mSocket, SIGNAL(connected()), this, SLOT(socketConnected()));
	mNextBlockSize = 0;
}

Connection::~Connection() {
	mSocket.close();
}

void Connection::readSocket() {
	static Message* mesg = NULL;
	QDataStream in(&mSocket);			
	while(mSocket.bytesAvailable()>0) {
		if(mesg==NULL) {
			if(mSocket.bytesAvailable() < sizeof(quint16)) {
				return;
			} 
			quint16 type = 0;
			in >> type;
			mesg = MessageFactory::getMessageByType(type);
		}
		if(mesg == NULL || mesg->enoughDataPresent(&mSocket)==false) {
			return;
		} 
		mesg->read(&mSocket);
		processMessage(mesg);
		emit messageReceived(this, mesg);
		delete mesg;
		mesg = NULL;
	}
}

void Connection::sendMessage(Message* mesg) {
	if(mSocket.state() != QAbstractSocket::ConnectedState) {
		std::cout << "Could not send message because the socket is not connected.\n";
		return;
	}
	mesg->write(&mSocket);

}


QTcpSocket* Connection::socket() {
	return &mSocket;
}

