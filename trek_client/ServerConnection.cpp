#include "ServerConnection.h"

ServerConnection::ServerConnection() {
	QObject::connect(Controller::self(), SIGNAL(serverConnectionAttempted()), this, SLOT(connect()));
	QObject::connect(Controller::self(), SIGNAL(serverDisconnect()), this, SLOT(disconnect()));

	QObject::connect(this, SIGNAL(alreadyConnected()), Controller::self(), SIGNAL(serverConnected()));
	QObject::connect(&mSocket, SIGNAL(connected()), Controller::self(), SIGNAL(serverConnected()));

	QObject::connect(&mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
	QObject::connect(&mSocket, SIGNAL(connected()), this, SLOT(connected()));
	QObject::connect(&mSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	QObject::connect(&mSocket, SIGNAL(readyRead()), this, SLOT(readSocket()));

	QObject::connect(this, SIGNAL(failedToConnect()), Controller::self(), SIGNAL(serverConnectionFailure()));
	QObject::connect(this, SIGNAL(connectionLost()), Controller::self(), SIGNAL(serverConnectionLost()));

}
ServerConnection::~ServerConnection() {
	mSocket.close();
}

void ServerConnection::initialize() {}
void ServerConnection::connect() {
	if(mSocket.state() == QAbstractSocket::ConnectedState) {
		emit alreadyConnected();
	} else

	//Home
        mSocket.connectToHost("localhost", 4133); //"98.236.170.236", 4132);

	//SliceHost
        //mSocket.connectToHost("174.143.168.229", 4133);

}
void ServerConnection::disconnect() {
	mSocket.disconnectFromHost();
}
void ServerConnection::sendMessage(Message* mesg) {
	if(mSocket.state() != QAbstractSocket::ConnectedState) {
		std::cout << "Could not send message because the socket is not connected.\n";
	}
	mesg->write(&mSocket);
}

bool ServerConnection::processMessage(Message* mesg) {
//	mesg->print();
	emit messageReceived(mesg);
	return true;
}

void ServerConnection::readSocket() {
	static Message* mesg = NULL;
	QDataStream in(&mSocket);
	while(mSocket.bytesAvailable() > 0) {
		if(mesg == NULL) {
			if(mSocket.bytesAvailable() < sizeof(quint16)) {
				return;
			}	
			quint16 type=0;
			in >> type;
			mesg = MessageFactory::getMessageByType(type);
		}

		if(mesg == NULL || mesg->enoughDataPresent(&mSocket)==false) {
			return;
		}

		mesg->read(&mSocket);
		processMessage(mesg);
			
		delete mesg;
		mesg = NULL;
	}
}


void ServerConnection::error() {
	std::cout << "Server Error!\n";
	emit failedToConnect();
}

void ServerConnection::connected() {}

void ServerConnection::disconnected() {
	emit connectionLost();
	//QApplication::exit(-1);
}
