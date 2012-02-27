#include "ClientData.h"

ClientData::ClientData(QObject* parent) {
	mSocket.setReadBufferSize(65535*2);
	connect(&mSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
	connect(&mSocket, SIGNAL(readyRead()), this, SLOT(readSocket()));
	connect(&mSocket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
	mNextBlockSize = 0;
	mId = -1;
	mCurrentLeader = NULL;
}
ClientData::~ClientData() {
	mSocket.close();
}

void ClientData::socketConnected() {}

void ClientData::socketDisconnected() {
	emit disconnected(this);
}

bool ClientData::processMessage(Message* mesg) {
	mesg->print();
	if(mesg->mType == AUTHENTICATION) {
		s2cAuthenticatedMessage mesg;	
		mesg.mIsSuccessful = true; //Authenticate here
		sendMessage(&mesg);
	}
	return true;
}

void ClientData::readSocket() {
	static Message* mesg = NULL;
	static bool isEncrypted=false;
	QDataStream in(&mSocket);			
	while(mSocket.bytesAvailable()>0) {
		if(mesg==NULL) {
			if(mSocket.bytesAvailable() < sizeof(quint16)) {
				return;
			} 
			quint16 type = 0;
			in >> type;
			if(type == 4321) {
				isEncrypted = true;
				std::cout << "Ecrypted MEssage\n";
				if(mSocket.bytesAvailable() < sizeof(quint16)) {
					return;
				}
				in >> type;
			}
			mesg = MessageFactory::getMessageByType(type);
		}
		if(mesg == NULL || mesg->mType == NO_OP) {
			//Disconnect the bugger
			std::cout << "Got a srange message; disconnecting the client\n";
			mSocket.disconnectFromHost();
			return;
		}
		if(mesg->enoughDataPresent(&mSocket)==false) {
			return;
		} 
		if(mesg->mType != NO_OP) {
			Message* heapMesg = MessageFactory::getMessageByType(mesg->mType);
			heapMesg->read(&mSocket);
		
			std::cout << "Client Data (.cpp) received: ";
			heapMesg->print();
			//processMessage(mesg);
			emit messageReceived(this, heapMesg);
		} else {
			std::cout << "Unrecognized Message, reading past data\n";
			mSocket.read(mesg->blockSize);
		}
		delete mesg;
		mesg = NULL;
	}
}

void ClientData::disconnect() {
	socket()->disconnectFromHost();	
}
void ClientData::sendMessage(Message* mesg) {
	if(mSocket.state() != QAbstractSocket::ConnectedState) {
		std::cout << "Could not send message because the socket is not connected.\n";
	}
	mesg->write(&mSocket);

}

void ClientData::socketStateChanged(QAbstractSocket::SocketState state) {
	if(state == QAbstractSocket::ConnectedState) {
		socketConnected();
	}
	if(state == QAbstractSocket::UnconnectedState) {
		deleteLater();
	}
}

QTcpSocket* ClientData::socket() {
	return &mSocket;
}

void ClientData::setUrl(const QUrl& url) {
	mUrl = url;
}

bool ClientData::isMoved() {
	return (!mMostRecentMove.mGuid.isNull());
}

s2cMoveMessage* ClientData::mostRecentMove() {
	return &mMostRecentMove;
}

void ClientData::setMostRecentMove(s2cMoveMessage mesg) {
	mMostRecentMove = mesg;
}
void ClientData::setLeader(ClientData* leader) {
	mCurrentLeader = leader;
}
ClientData* ClientData::leader() {
	return mCurrentLeader;
}

void ClientData::setPartyColor(const QColor& c) {
	mPartyColor = c;
}
QColor ClientData::partyColor() {
	return mPartyColor;
}	

QUrl ClientData::currentUrl() {
	return mUrl;
}
void ClientData::setGuid(QUuid guid) {
	mGuid = guid;
}
QUuid ClientData::guid() {
	return mGuid;
}
void ClientData::setId(unsigned int id) {
	mId = id;
}
unsigned int ClientData::id() {
	return mId;
}

void ClientData::setUsername(const QString& username) {
	std::cout << "Setting username: " << username.toStdString() << "\n";
	mUsername = username;
}
QString ClientData::username() {
	return mUsername;
}

void ClientData::setEmailAddress(const QString& emailAddy) {
	mEmailAddress = emailAddy;
}
QString ClientData::emailAddress() {
	return mEmailAddress;
}

QString ClientData::nonce() {
	return mNonce;
}	
void ClientData::setNonce(const QString& nonce) {
	mNonce = nonce; 
}
