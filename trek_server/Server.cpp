#include "Server.h"

Server::Server(QObject* parent) : QTcpServer(parent) {
	
}

void Server::incomingConnection(int socketId) {
	std::cout << "Connection accepted\n";
	ClientData* newClient = new ClientData();
	newClient->socket()->setSocketDescriptor(socketId);
	/*newClient->socket()->setLocalCertificate("/home/jkross/cert.pem");
	  newClient->socket()->setPrivateKey("/home/jkross/key.pem", QSsl::Rsa, QSsl::Pem, "trekpass");
	  newClient->socket()->startServerEncryption();
	  newClient->socket()->waitForEncrypted();
	  std::cout << newClient->socket()->errorString().toStdString() << "\n";
	 */
	connect(newClient, SIGNAL(messageReceived(ClientData*, Message*)), this, SLOT(processClientMessage(ClientData*, Message*)));
	connect(newClient, SIGNAL(disconnected(ClientData*)), this, SLOT(clientDisconnected(ClientData*)));
}
void Server::sendToUrl(const QUrl& url, Message* mesg) {
	QList<ClientData*> data = mDataByUrl[url];
	for(int i=0; i<data.count(); i++) {
		data[i]->sendMessage(mesg);
	}
}
void Server::clientDisconnected(ClientData* client) {
	std::cout << "Deleting client with guid: " << client->guid().toString().toStdString() << "\n";

	if(client->guid().isNull() == true) {
		client->deleteLater();
		return;
	}

	informRelationsOfStatusChange(client->guid(), OFFLINE);

	if(client->guid().isNull() == false)
		mDataByGuid.remove(client->guid());
	if(mDataById.contains(client->id())) 
		mDataById.remove(client->id());
	if(client->currentUrl().isEmpty()==false) {
		mDataByUrl[client->currentUrl()].removeOne(client);
	}


	s2cRemoveUsersMessage informDeparture;
	informDeparture.mUsersCount = 1;
	informDeparture.mGuids.append(client->guid());
	informDeparture.mUrls.append(QUrl());
	sendToUrl(client->currentUrl(), &informDeparture);

	if(mExpeditionMap.contains(client) || client->leader()) {
		informPartyOfDeparture(client);
	}

	QSqlQuery query;
	query.prepare("select count(user_id) from history where user_id = :user_id");
	query.bindValue(":user_id", client->id());
	query.exec();
	query.next();
	if(query.value(0).toInt() > 50) {
		query.prepare("DELETE FROM history WHERE id < (SELECT MIN(id) FROM (SELECT id FROM history WHERE user_id = :user_id0 ORDER BY id DESC LIMIT 50) as temp) AND user_id = :user_id1");
		query.bindValue(":user_id0", client->id());
		query.bindValue(":user_id1", client->id());
		query.exec();
	}

	client->deleteLater();	
}

void Server::processClientMessage(ClientData* client, Message* mesg) {
	if(mesg->mType == AUTHENTICATION) {
		processClientAuthentication(client, (AuthenticationMessage*)mesg);
	} else if(mesg->mType == c2sSPEAK) {
		processClientSpeak(client, (c2sSpeakMessage*)mesg);
	} else if(mesg->mType == c2sPERSONAL_SPEAK) {
		processClientPersonalSpeak(client, (c2sPersonalSpeakMessage*)mesg);
	} else if(mesg->mType == c2sLOCALE_CHANGED) {
		processClientLocaleChanged(client, (c2sLocaleChangedMessage*)mesg);	
	} else if(mesg->mType == c2sSIGNUP) {
		processClientSignup(client, (c2sSignupMessage*)mesg);
	} else if(mesg->mType == c2sCHANGE_APPEARANCE) {
		processClientChangeAppearance(client, (c2sChangeAppearanceMessage*)mesg);
	} else if(mesg->mType == c2sREQUEST_APPEARANCE) {
		processClientRequestAppearance(client, (c2sRequestAppearanceMessage*)mesg);
	} else if(mesg->mType == c2sMOVE) {
		processClientMove(client, (c2sMoveMessage*)mesg);
	} else if(mesg->mType == c2sFOLLOW_USER) {
		processClientFollowUser(client, (c2sFollowUserMessage*)mesg);
	} else if(mesg->mType == c2sUNFOLLOW_USER) {
		processClientUnfollowUser(client, (c2sUnfollowUserMessage*)mesg);
	} else if(mesg->mType == UNIVERSAL_REQUEST) {
		//QtConcurrent::run(this, &Server::processUniversalRequest, client, (UniversalRequestMessage*)mesg);
		processUniversalRequest(client, (UniversalRequestMessage*)mesg); 
	} else if(mesg->mType == c2sCHANGE_PROFILE) {
		processClientChangeProfile(client, (c2sChangeProfileMessage*)mesg);
	} else if(mesg->mType == c2sADD_BOOKMARKS) {
		processClientAddBookmarks(client, (c2sAddBookmarksMessage*)mesg);
	} else if(mesg->mType == c2sREMOVE_BOOKMARKS) {
		processClientRemoveBookmarks(client, (c2sRemoveBookmarksMessage*)mesg);
	} else if(mesg->mType == c2sADD_HISTORY) {
		//QtConcurrent::run(this, &Server::processClientAddHistory, client, (c2sAddHistoryMessage*)mesg);
		processClientAddHistory(client, (c2sAddHistoryMessage*)mesg);
	} else if(mesg->mType == c2sASK_JOIN) {
		processClientAskJoin(client, (c2sAskJoinMessage*)mesg);
	} else if(mesg->mType == c2sASK_UNJOIN) {
		processClientAskUnjoin(client, (c2sAskUnjoinMessage*)mesg);
	} else if(mesg->mType == c2sBETA_INVITE) {
		//processClientBetaInvite(client, (c2sBetaInviteMessage*)mesg);
	} else if(mesg->mType == c2sCONF_KEY) {
		//processConfKey(client, (c2sConfKeyMessage*)mesg);
	} else if(mesg->mType == c2sRESET_PASSWORD) {
		processClientResetPassword(client, (c2sResetPasswordMessage*)mesg);
	} else if(mesg->mType == c2sCHANGE_PASSWORD) {
		processClientChangePassword(client, (c2sChangePasswordMessage*)mesg);
	} else if(mesg->mType == c2sADMIN_COMMAND) {
		processClientAdminCommand(client, (c2sAdminCommandMessage*)mesg);
	} else if(mesg->mType == c2sCHANGE_MOTD) {
		processChangeMotd(client, (c2sChangeMotdMessage*)mesg);
	} else {
		std::cout << "Unrecognized Message Type on the Server: " << mesg->mType << "\n";
	}
}

bool Server::connectToDB() {
	if(QSqlDatabase::database().isOpen()) {	
		QSqlQuery query;
		query.exec("SELECT 1");
		if(query.next()) {
			return true;
		} else {
			QSqlDatabase::database().close();
		}
	}
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName("localhost");
	db.setDatabaseName("pluto_db_test");
	//db.setDatabaseName("pluto_db");
	db.setUserName("jeremykross");
	db.setPassword("aug81285");
	return db.open();
}

unsigned int Server::helperGetOrAddPage(const QString& name, const QString& url) {
	QSqlQuery query;
	query.prepare("SELECT id FROM page WHERE url = :url");
	query.bindValue(":url", url);
	query.exec();
	if(query.size() == 0) {
		query.prepare("INSERT INTO page(url, name) VALUES(:url, :name)");
		query.bindValue(":url", url);
		std::cout << "Binding name: " << name.toStdString() << "\n";
		query.bindValue(":name", name);
		query.exec();
		query.prepare("SELECT id FROM page WHERE url = :url");
		query.bindValue(":url", url);
		query.exec();
	}
	query.next();
	unsigned int pageId = query.value(0).toUInt();
	return pageId;	
}

void Server::processChangeMotd(ClientData* client, c2sChangeMotdMessage* mesg) {
	CONNECT_TO_DB;
	QSqlQuery query;
	query.prepare("UPDATE plutoite SET motd = :motd WHERE user_id = :id"); 
	query.bindValue(":id", client->id());
	query.bindValue(":motd", mesg->mMessage);
	query.exec();
}

void Server::processClientAdminCommand(ClientData* client, c2sAdminCommandMessage* mesg) {
	QSettings settings("Kukundu Soft", "PlutoServer");	
	settings.beginGroup(QString("%1").arg(client->id()));
	bool isAdmin = settings.value("isAdmin").toBool();
	settings.endGroup();
	if(isAdmin== false) return;		
	if(mesg->mCommand == c2sAdminCommandMessage::MOTD) {
				QSettings s("Kukundu Soft", "PlutoServer");
				s.setValue("motd", mesg->mOption);
				return;
	}

	QList<ClientData*> sendTo;
	if(mesg->mUsername == "[all]") {
		QMap<unsigned int, ClientData*>::const_iterator i;	
		for(i = mDataById.begin(); i!=mDataById.end(); i++) {
			sendTo.append(i.value());
		}
	} else {
		sendTo.append(mDataById.value(helperIdForUsername(mesg->mUsername)));
	}
	for(int i=0; i<sendTo.count(); i++) {
		ClientData* to = sendTo.value(i);
		if(to) {
			if(mesg->mCommand == c2sAdminCommandMessage::SAY) {
				helperSendTextMessage(to, mesg->mOption);
			} else if(mesg->mCommand == c2sAdminCommandMessage::KICK) {
				QString taunt;
				if(mesg->mOption.isEmpty() == false) {
					taunt =mesg->mOption;
				} else {
					taunt = "You're About to Be Kicked.";
				}
				helperSendTextMessage(to, taunt);
				QTimer::singleShot(5000, to, SLOT(disconnect()));
			} else if(mesg->mCommand == c2sAdminCommandMessage::BLOCK) {
				QString taunt;
				if(mesg->mOption.isEmpty() == false) {
					taunt = mesg->mOption;
				} else {
					taunt = "You've Been Temporarily Banned, Try Logging in Again in 24 Hours";
				}
				helperSendTextMessage(to, taunt);
				CONNECT_TO_DB;
				QSqlQuery query;
				query.prepare("UPDATE plutoite SET authenticated = -1 where user_name = :username");	
				query.bindValue(":username", to->username());
				query.exec();
				QTimer::singleShot(5000, to, SLOT(disconnect()));
			} else if(mesg->mCommand == c2sAdminCommandMessage::SPEAK) {
				c2sSpeakMessage adminMesg;	
				adminMesg.mCommunique = mesg->mOption;
				processClientSpeak(to, &adminMesg);
			} else if(mesg->mCommand == c2sAdminCommandMessage::MOVE_TO) {	
				s2cPartyMoveMessage moveIt;
				moveIt.mUrl = QUrl(mesg->mOption);
				to->sendMessage(&moveIt);
			}
		}
	}
}
void Server::processClientChangePassword(ClientData* client, c2sChangePasswordMessage* mesg) { 
	std::cout << "Change Received: " << mesg->mNewPass.toStdString() << "\n";
	CONNECT_TO_DB;
	QSqlQuery query;
	query.prepare("UPDATE plutoite SET password = :password WHERE user_id = :id");
	query.bindValue(":password", mesg->mNewPass.toAscii());
	query.bindValue(":id", client->id());
	query.exec();

}

void Server::processClientResetPassword(ClientData* client, c2sResetPasswordMessage* mesg) {
	std::cout << "Resetting for: " << client->emailAddress().toStdString() << "\n";
	QString password = helperResetPassword(client);
	QString emailMesg;
	emailMesg = "Hello again.  Forgot your password, did you?\n\nYour new password is:     ";
	emailMesg += password;		
	emailMesg += "\n\n--------------------------------------\nQuestions, Comments?  Just reply to this email and we'll kindly get back to you.";
	helperSendEmail(client->emailAddress(), "Trek: Password Reset", emailMesg); 

}
/*void Server::processConfKey(ClientData* client, c2sConfKeyMessage* mesg) {
  CONNECT_TO_DB;
  QSqlQuery query;
  query.prepare("SELECT authentication_key FROM plutoite WHERE user_id = :id");
  query.bindValue(":id", client->id());
  query.exec();
  query.next();
  QString authKey = query.value(0).toString();
  std::cout << "Looking for: " << authKey.toStdString() << " for client " << client->id() << "\n";
  UniversalRequestMessage reply;
  if(mesg->mKey == authKey && authKey.isEmpty() == false) {
  reply.mRequest = "auth_valid";
  query.prepare("UPDATE plutoite SET authenticated = true WHERE user_id = :id");
  query.bindValue(":id", client->id());
  query.exec();
  client->sendMessage(&reply);
  helperAllowLogin(client);
  } else {
  reply.mRequest = "auth_invalid";
  client->sendMessage(&reply);
  }
  }*/
/*void Server::processClientBetaInvite(ClientData* client, c2sBetaInviteMessage* mesg) {
  std::cout << "Sending an Invite to: " << mesg->mToEmail.toStdString() << "\n";
  QSqlQuery query;
  query.prepare("SELECT email_address, first_name, last_name FROM plutoite WHERE user_id = :user_id"); 
  query.bindValue(":user_id", client->id());
  query.exec();
  if(!query.next()) return;

  QString clientEmail = query.value(0).toString();
  QString clientFirstName = query.value(1).toString();
  QString clientLastName = query.value(2).toString();

  QProcess p;
	QString program = "/home/jkross/Projects/pluto_betainvite/pluto_betainvite";
	QStringList args;
	args << "-t" << mesg->mToEmail;
	args << "-ia" << clientEmail;

	if(mesg->mFromName.isEmpty() == false) 
		args << "-in" << mesg->mFromName;	
	else
		args << "-in" << clientFirstName + " " + clientLastName;

	args << "-ic" << "3";
	args << "-e";
	std::cout << QProcess::startDetached(program, args) << "\n";
}*/

void Server::processClientAddHistory(ClientData* client, c2sAddHistoryMessage* mesg) {
	std::cout << "Called async!\n";
	CONNECT_TO_DB;
	unsigned int pageId = helperGetOrAddPage(mesg->mNames[0], mesg->mUrls[0].toString());
	QSqlQuery query;
	query.prepare("INSERT INTO history(user_id, page_id) VALUES(:user_id, :page_id)");	
	query.bindValue(":user_id", client->id());
	query.bindValue(":page_id", pageId);
	//query.bindValue(":name", mesg->mNames[0]);
	query.exec();

	
}
void Server::processClientAddBookmarks(ClientData* client, c2sAddBookmarksMessage* mesg) {
	CONNECT_TO_DB;
	QSqlQuery query;
/*	query.prepare("SELECT id FROM page WHERE url = :url");
	query.bindValue(":url", mesg->mUrls[0].toString());
	query.exec();
	if(query.size() == 0) {
		query.prepare("INSERT INTO page(url) VALUES(:url)");
		query.bindValue(":url", mesg->mUrls[0].toString());
		query.exec();
		query.prepare("SELECT id FROM page WHERE url = :url");
		query.bindValue(":url", mesg->mUrls[0].toString());
		query.exec();
	}
	query.next();
	*/

	unsigned int pageId = helperGetOrAddPage(mesg->mNames[0],mesg->mUrls[0].toString());
	unsigned int userId = client->id(); 
	query.prepare("INSERT INTO favorite(user_id, page_id) VALUES(:user_id, :page_id)");
	query.bindValue(":user_id", userId);
	query.bindValue(":page_id", pageId);
	//query.bindValue(":name", mesg->mNames[0]);
	query.exec();
}

void Server::processClientRemoveBookmarks(ClientData* client, c2sRemoveBookmarksMessage* mesg) {
	std::cout << "Remvoing bookmark\n";
	CONNECT_TO_DB;
	QSqlQuery query;
	query.prepare("DELETE FROM favorite WHERE user_id = :user_id AND page_id = (SELECT id FROM page WHERE url = :url)");
	query.bindValue(":user_guid", client->id());
	query.bindValue(":url", mesg->mUrls[0].toString());
	query.exec();
}

void Server::processClientAskUnjoin(ClientData* client, c2sAskUnjoinMessage* mesg) {
	UniversalRequestMessage askUnjoinRequest;
	askUnjoinRequest.mGuid = client->guid();
	askUnjoinRequest.mRequest = "expedition_unask";
	QMap<ClientData*, bool> map = mExpeditionMap[client];
	for(int i=0; i<mesg->mGuids.count(); i++) {
		ClientData* cd = mDataByGuid.value(mesg->mGuids[i]);
		if(cd) {
			if(map.contains(cd)) {
				if(map[cd]) {
					informPartyOfDeparture(cd);
					cd->sendMessage(&askUnjoinRequest);
				}
				//map.remove(cd);
			}
		}
	}
	//mExpeditionMap.insert(client, map);
		
}

void Server::processClientAskJoin(ClientData* client, c2sAskJoinMessage* mesg) {
	UniversalRequestMessage askJoinRequest;
	askJoinRequest.mGuid = client->guid();
	askJoinRequest.mRequest = "expedition_ask";

	QMap<ClientData*, bool> map = mExpeditionMap[client];
	for(int i=0; i<mesg->mGuids.count(); i++) {
		ClientData* cd = mDataByGuid.value(mesg->mGuids[i]);
		if(cd) {
			if(map.contains(cd) == false) {
				map.insert(cd, false);
				cd->sendMessage(&askJoinRequest);
			}
		}
	}
	mExpeditionMap.insert(client, map);
	client->setPartyColor(mesg->mColor);
	AppearanceMessage appearanceMesg;
	appearanceMesg.mGuid = client->guid();
	appearanceMesg.mPartyColor = mesg->mColor;
	appearanceMesg.mIsLeader = true;
	appearanceMesg.hasPartyColor = true;
	sendToUrl(client->currentUrl(), &appearanceMesg);
}
QString Server::helperResetPassword(ClientData* client) {
	QString password = "reset_" + helperMakeNonce(7);
	QSqlQuery query;
	query.prepare("UPDATE plutoite SET password = :password WHERE email_address = :email");
	query.bindValue(":password", password);
	query.bindValue(":email", client->emailAddress());
	query.exec();
	return password;
}
QString Server::helperMakeNonce(int nonceSize) {
	QString allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ012345789";	
	srand(time(NULL));
	QString nonce;
	for(int i=0; i<nonceSize; i++) {
		nonce += allowedChars.at(rand()%allowedChars.count());
	}
	return nonce;	
}
int Server::helperIdForUsername(const QString& username) {
	CONNECT_TO_DB_VAL;
	QSqlQuery query;
	query.prepare("SELECT user_id FROM plutoite WHERE user_name = :name");
	query.bindValue(":name", username);
	query.exec();
	if(query.next()) {
		return query.value(0).toInt();
	} else {
		return -1;
	}
}
int Server::helperIdForGuid(const QUuid& guid) {
	CONNECT_TO_DB_VAL;
	QSqlQuery query;
	query.prepare("SELECT user_id FROM plutoite WHERE user_guid = :guid");
	query.bindValue(":guid", guid.toString());
	query.exec();
	if(query.next()) {
		return query.value(0).toInt();
	} else {
		return -1;
	}
}

void Server::helperSendTextMessage(ClientData* client, const QString& message) {
	s2cSayMessage mesg;
	mesg.mMessage = message;
	client->sendMessage(&mesg);
}
void Server::helperChangeRelationship(const QUuid& follower, const QUuid& followee, const QString& status) {
	CONNECT_TO_DB;
	QSqlQuery query;
	int followerId = helperIdForGuid(follower);
	int followeeId = helperIdForGuid(followee);
	query.prepare("INSERT INTO friendship VALUES(:followerId, :followeeId, :status)");
	query.bindValue(":followerId", followerId);
	query.bindValue(":followeeId", followeeId);
	query.bindValue(":status", status);
	bool success = query.exec();
	if(success == false) {
		query.prepare("UPDATE friendship SET status = :status WHERE follower_id = :followerId AND followee_id = :foloweeId");
		query.bindValue(":followerId", followerId);
		query.bindValue(":followeeId", followeeId);
		query.bindValue(":status", status); 
		query.exec();
	}
	ClientData* followerClient =NULL;
	if(mDataById.contains(followerId)) followerClient= mDataById.value(followerId);
	ClientData* followeeClient =NULL; 
	if(mDataById.contains(followeeId)) followeeClient= mDataById.value(followeeId);
	
	if(followerClient) {
		if(status == "a") {
			s2cAddFollowedMessage mesg;
			mesg.mOwnerGuid = follower;
			mesg.mGuids.append(followee);
			followerClient->sendMessage(&mesg);
		} else if(status == "b") {
			s2cRemoveFollowedMessage mesg;
			mesg.mOwnerGuid = follower;
			mesg.mGuids.append(followee);
			followerClient->sendMessage(&mesg);	
			/*if(followeeClient) {
				helperSendTextMessage(followerClient, followeeClient->username() + " Has Denied You as a Fan.");
			}*/
		}
	}
	if(followeeClient) {
		if(status == "a") {
			s2cAddFansMessage mesg;
			mesg.mOwnerGuid = followee;
			mesg.mGuids.append(follower);
			followeeClient->sendMessage(&mesg);
		} else if(status == "b") {
			/*s2cRemoveFansMessage mesg;
			mesg.mOwnerGuid = followee;
			mesg.mGuids.append(follower);
			followeeClient->sendMessage(&mesg);*/	
		}
	}
}
void Server::processUniversalRequest(ClientData* client, UniversalRequestMessage* mesg) {
	if(mesg->mRequest.compare("kick") ==0) {
		QSettings settings("Kukundu Soft", "PlutoServer");	
		settings.beginGroup(QString("%1").arg(client->id()));
		bool isAdmin = settings.value("isAdmin").toBool();
		settings.endGroup();
		if(isAdmin==false) return;
		ClientData* toKick = mDataByGuid.value(mesg->mGuid);
		if(toKick) {
			helperSendTextMessage(toKick, "You're About to Be Kicked.");
			QTimer::singleShot(5000, toKick, SLOT(disconnect()));
		}
	} else if(mesg->mRequest.compare("ic")==0) {
		/*CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("select invite_count from beta_invite where to_email_address = :emailAddy");
		query.bindValue(":emailAddy", client->emailAddress());
		query.exec();
		if(!query.next()) return;
		int inviteCount = query.value(0).toInt();
		s2cInviteCountMessage reply;
		reply.mCount = inviteCount;
		client->sendMessage(&reply);*/		
	} else if(mesg->mRequest.compare("pf") == 0) {
		CONNECT_TO_DB;	
		QSqlQuery query;
		query.prepare("SELECT user_guid, user_name FROM plutoite WHERE user_id IN (SELECT follower_id FROM friendship WHERE followee_id=:clientId AND status='p')"); 
		query.bindValue(":clientId", client->id()); 
		query.exec();
		s2cPendingFollowersMessage reply;
		while(query.next() && query.isValid()) {
			reply.mGuids.append(QUuid(query.value(0).toString()));
			reply.mUsernames.append(query.value(1).toString());
		}
		if(reply.mGuids.size()) 
			client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("allow_f") ==0) {
		helperChangeRelationship(mesg->mGuid, client->guid(), "a");	
	} else if(mesg->mRequest.compare("deny_f") == 0) {
		helperChangeRelationship(mesg->mGuid, client->guid(), "d");
	} else if(mesg->mRequest.compare("block") == 0) {
		helperChangeRelationship(mesg->mGuid, client->guid(), "b");
	} else if(mesg->mRequest.compare("pfing") == 0) {
		CONNECT_TO_DB;	
		QSqlQuery query;
		query.prepare("SELECT user_guid, user_name FROM plutoite WHERE user_id IN (SELECT followee_id FROM friendship WHERE follower_id=:clientId AND status='p')"); 
		query.bindValue(":clientId", client->id()); 
		query.exec();
		s2cPendingFollowingMessage reply;
		while(query.next() && query.isValid()) {
			reply.mGuids.append(QUuid(query.value(0).toString()));
			reply.mUsernames.append(query.value(1).toString());
		}
		if(reply.mGuids.size()) 
			client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("nonce") == 0) {
		std::cout << "Generating a nonce\n";
		s2cNonceMessage reply;
		reply.mNonce = helperMakeNonce(); 
		client->setNonce(reply.mNonce);
		client->sendMessage(&reply);
		std::cout << "Sent the nonce\n";
	} else if(mesg->mRequest.compare("status") == 0) {
		s2cStatusMessage reply;
		reply.mGuid = mesg->mGuid;
		if(mDataByGuid.contains(mesg->mGuid)) {
			reply.mStatus = ONLINE;	
		} else {
			reply.mStatus = OFFLINE;	
		}
		if(mDataByGuid.contains(mesg->mGuid)) {
			reply.mUrl = mDataByGuid[mesg->mGuid]->currentUrl().toString(); 
		}
		client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("fans") == 0) {
		CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("SELECT user_guid, user_name FROM plutoite WHERE user_id IN (SELECT follower_id FROM friendship WHERE status = 'a' AND followee_id IN (SELECT user_id FROM plutoite WHERE user_guid = :follower))"); 
		query.bindValue(":follower", mesg->mGuid.toString());
		query.exec();
		
		QList<QUuid> guids;
		QList<QString> usernames;

		while(query.next() && query.isValid()) {
			guids.append(QUuid(query.value(0).toString()));
			usernames.append(query.value(1).toString());
		}

		s2cAddFansMessage reply;
		reply.mOwnerGuid = mesg->mGuid;
		reply.mUsersCount = guids.size();
		reply.mGuids = guids;
		reply.mUsernames = usernames; 
		client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("followed") == 0) {
		CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("SELECT user_guid, user_name FROM plutoite WHERE user_id IN (SELECT followee_id FROM friendship WHERE status = 'a' AND follower_id IN (SELECT user_id FROM plutoite WHERE user_guid = :follower))");
		query.bindValue(":follower", mesg->mGuid.toString());
		query.exec();

		QList<QUuid> guids;
		QList<QString> usernames;

		while(query.next() && query.isValid()) {
			guids.append(QUuid(query.value(0).toString()));
			usernames.append(query.value(1).toString());
		}

		s2cAddFollowedMessage reply;
		reply.mOwnerGuid = mesg->mGuid;
		reply.mUsersCount = guids.size();
		reply.mGuids = guids;
		reply.mUsernames = usernames;
		client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("profile")==0) {
		std::cout << "Profile for: " << mesg->mGuid.toString().toStdString() << "\n";
		CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("SELECT occupational_info, hobbies, favorite_culture, general_info,email_address,sex,birth_date,motd FROM plutoite WHERE user_guid = :guid");	
		query.bindValue(":guid", mesg->mGuid.toString());
		query.exec();
		query.next();

		s2cProfileMessage reply;
		reply.mGuid = mesg->mGuid;
		reply.mOccupationInfo = query.value(0).toString();
		reply.mHobbies = query.value(1).toString();
		reply.mFavoriteCulture = query.value(2).toString();
		reply.mGeneralInfo = query.value(3).toString();
		reply.mEmailAddress = query.value(4).toString();
		reply.mGender = query.value(5).toString();	
		reply.mBirthDate = query.value(6).toDate();
		reply.mMotd = query.value(7).toString();

		client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("favorites") == 0) {
		CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("SELECT page.name, page.url FROM page, favorite WHERE favorite.page_id = page.id AND favorite.user_id = (SELECT user_id FROM plutoite WHERE user_guid = :guid)"); 
		query.bindValue(":guid", mesg->mGuid.toString());
		query.exec();
		s2cAddBookmarksMessage reply;
		reply.mGuid = mesg->mGuid;
		while(query.next() && query.isValid()) {
			reply.mNames.append(query.value(0).toString());
			reply.mUrls.append(QUrl(query.value(1).toString()));
		}
		client->sendMessage(&reply);	
 	} else if(mesg->mRequest.compare("cls_hist") == 0) {
		CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("DELETE FROM history WHERE user_id = :id");
		query.bindValue(":id", client->id());
		query.exec();
	} else if(mesg->mRequest.compare("history") == 0) {
		CONNECT_TO_DB;
		QSqlQuery query;
		query.prepare("SELECT page.name, page.url FROM page, history WHERE history.page_id = page.id AND history.user_id = (SELECT user_id FROM plutoite WHERE user_guid = :guid) ORDER BY history.id"); 
		query.bindValue(":guid", mesg->mGuid.toString());
		query.exec();
		s2cAddHistoryMessage reply;
		reply.mGuid = mesg->mGuid;
		while(query.next() && query.isValid()) {
			reply.mNames.append(query.value(0).toString());
			reply.mUrls.append(QUrl(query.value(1).toString()));
		}
		client->sendMessage(&reply);
	} else if(mesg->mRequest.compare("locations") == 0) {
		QList<ClientData*> data = mDataByUrl[client->currentUrl()];
		for(int i=0; i<data.count(); i++) {
			if(data[i]->isMoved())
				client->sendMessage(data[i]->mostRecentMove());
		}
		return;	
	} else if(mesg->mRequest.compare("expedition_accept") == 0) {
		//Check if there's a pending request
		if(client->leader() && client->leader()->guid() == mesg->mGuid) {
			return;
		}
		if(client->leader() || mExpeditionMap.contains(client)) {
			informPartyOfDeparture(client);
		}
		ClientData* leader = mDataByGuid.value(mesg->mGuid);
		if(leader) {
			if(mExpeditionMap.contains(leader)) {
				QMap<ClientData*, bool> party = mExpeditionMap.value(leader);
				if(party.contains(client)) {
					party.insert(client, true);
					mExpeditionMap.insert(leader, party);
					client->setLeader(leader);
					client->setPartyColor(leader->partyColor());
					informPartyOfArrival(client);

					s2cPartyAddUsersMessage addUsersMesg;	
					QList<QUuid> guids;
					QList<QString> usernames;
					guids.append(leader->guid());
					usernames.append(leader->username());
					QMap<ClientData*, bool>::const_iterator i;
					for(i=party.constBegin(); i!=party.constEnd(); i++) {
						if(i.key() && i.value()) {
							guids.append(i.key()->guid());
							usernames.append(i.key()->username());		
						}
					}
					addUsersMesg.mUsersCount = guids.count();
					addUsersMesg.mGuids = guids;
					addUsersMesg.mUsernames = usernames;
					
					client->sendMessage(&addUsersMesg);		
					AppearanceMessage appearanceMesg;
					appearanceMesg.hasPartyColor = true;
					appearanceMesg.mGuid = client->guid();
					appearanceMesg.mPartyColor = client->partyColor();
					client->sendMessage(&appearanceMesg);

					if(client->currentUrl() != leader->currentUrl()) {
						s2cPartyMoveMessage moveMesg;
						moveMesg.mUrl = leader->currentUrl();
						client->sendMessage(&moveMesg);
					} else {
						sendToUrl(client->currentUrl(), &appearanceMesg);
					}

					/*AppearanceMessage appearanceMesg;
					appearanceMesg.hasPartyColor = true;
					appearanceMesg.mGuid = client->guid();
					appearanceMesg.mPartyColor = client->partyColor();
					client->sendMessage(&appearanceMesg);
					leader->sendMessage(&appearanceMesg);
					appearanceMesg.mGuid = leader->guid();
					appearanceMesg.mIsLeader = true;
					client->sendMessage(&appearanceMesg);*/
				}
			} else {
				std::cout << "informing client the trek is closed\n";
				//The LEader has disbanded the trek?
				s2cPartyRemoveUsersMessage reply;
				reply.mUsersCount = 1;
				reply.mGuids.append(mesg->mGuid);
				client->sendMessage(&reply);
					
			}
		}
	} else if(mesg->mRequest.compare("expedition_deny") == 0) {
		//Check if there's a pending request
		ClientData* leader = mDataByGuid.value(mesg->mGuid);
		if(leader) {
			if(mExpeditionMap.contains(leader)) {
				QMap<ClientData*, bool> party = mExpeditionMap.value(leader);
				if(client != leader) {
					if(party.contains(client)) {
						if(party[client]) {
							informPartyOfDeparture(client);
						}
					}
				} else {
					informPartyOfDeparture(leader);
				}

			} else { }
		}	
	} 
}


void Server::helperAllowLogin(ClientData* client) {
	CONNECT_TO_DB;
	QSqlQuery query;

	s2cAuthenticatedMessage reply;
	reply.mIsSuccessful = true;
	reply.mGuid = client->guid(); //guid;//QUuid::createUuid();
	reply.mUsername = client->username();
	mDataByGuid.insert(client->guid(), client);
	mDataById.insert(client->id(), client);
	informRelationsOfStatusChange(client->guid(), ONLINE);
	query.prepare("UPDATE plutoite SET last_login = curdate() WHERE user_id = :id"); 
	query.bindValue(":id", client->id());
	query.exec();
	client->sendMessage(&reply);

}
void Server::processClientAuthentication(ClientData* client, AuthenticationMessage* mesg) {
	CONNECT_TO_DB;
	QSqlQuery query;
	query.exec("SELECT email_address, user_name, password, user_guid, user_id, authenticated, authentication_key, last_login FROM plutoite WHERE email_address = '" + mesg->mUsername + "'");
	query.next();
	QString emailAddress = query.value(0).toString();
	QString userName = query.value(1).toString();
	QString password = query.value(2).toString();
	QUuid guid = QUuid(query.value(3).toString());
	unsigned int id = query.value(4).toUInt();
	int isAuthenticated = query.value(5).toInt();
	QString authenticationKey = query.value(6).toString();
	QDate lastLogin = query.value(7).toDate();

	QString hashedPass = QString(QCryptographicHash::hash((password+client->nonce()).toAscii(), QCryptographicHash::Sha1));
	std::cout << "Password on Disk: " << password.toStdString() << "\n";
	std::cout << "nonce: " << client->nonce().toStdString() << "\n";
	std::cout << "Password Sent: " << mesg->mPassword.toStdString() << "\n";
	std::cout << "Hashed Pass: " << hashedPass.toStdString() << "\n";

	client->setEmailAddress(emailAddress);
	std::cout << "Authenticated: " << isAuthenticated << "\n";
	if(emailAddress.isEmpty() == false && (lastLogin.isNull() || (emailAddress == mesg->mUsername && password.isEmpty() == false && hashedPass.toAscii() == mesg->mPassword.toAscii())) && !mDataByGuid.contains(guid) && isAuthenticated != -1) {
		client->setGuid(guid);
		client->setUsername(userName); //((AuthenticationMessage*)mesg)->mUsername);
		client->setId(id);
		if(lastLogin.isNull() == false && isAuthenticated == false) {
			QSqlQuery query;
			query.prepare("UPDATE plutoite SET authenticated = true WHERE user_id = :id");
			query.bindValue(":id", client->id());
			query.exec();
		}
		/*if(lastLogin.isNull() == false && isAuthenticated == false) {
			//Need to request authentication first!
			UniversalRequestMessage reply;
			reply.mRequest = "auth_key";
			client->sendMessage(&reply);
			return;
		}*/
		helperAllowLogin(client);
		QSettings s("Kukundu Soft", "PlutoServer");
		//s.beginGroup(QString("%1").arg(client->id()));
		//s.setValue("isAdmin", true);
		//s.endGroup();
		if(s.contains("motd")) {
			helperSendTextMessage(client, "motd:" + s.value("motd").toString());
		}
		/*reply.mIsSuccessful = true;
		reply.mGuid = guid;//QUuid::createUuid();
		reply.mUsername = userName;
		mDataByGuid.insert(client->guid(), client);
		mDataById.insert(client->id(), client);
		informRelationsOfStatusChange(client->guid(), ONLINE);
		query.prepare("UPDATE plutoite SET last_login = curdate() WHERE user_id = :id"); 
		query.bindValue(":id", id);
		query.exec();
		*/
	} else {
		s2cAuthenticatedMessage reply;
		reply.mIsSuccessful = false;
		reply.mPWIncorrect = !emailAddress.isEmpty() && (mesg->mPassword.toAscii() != hashedPass.toAscii());
		if(mDataByGuid.contains(guid) && guid.isNull() == false) {
			reply.mCause = "User already logged in.";
			if(reply.mPWIncorrect == false) {
				reply.mCause += "<br/>(You've been logged off in the other location, please try again.)";
				ClientData* alreadyConnected = mDataByGuid.value(guid);
				alreadyConnected->socket()->disconnectFromHost();
			}
		} else if(reply.mPWIncorrect) 
			reply.mCause = "The Password is Incorrect.";
		else if(isAuthenticated == -1) {
			reply.mCause = "You've Been Temporarily Banned.";
		} else {
			reply.mCause = "Could not authenticate the given credentials.";
		}
		client->sendMessage(&reply);
	}

}
void Server::processClientSignup(ClientData* client, c2sSignupMessage* mesg) {
	//QMessageBox::information(NULL, "client", "signup");
	if(connectToDB()==false) {
		//	QMessageBox::information(NULL, "Database Error", QSqlDatabase::database().lastError().text());
		return;
	}
	s2cSignupResponseMessage reply;
	bool foundEmail = false;
	bool foundUsername = false;
	bool authenticated = false;
//	bool hasKey = false;
//	bool keyMatches = false;

	if(mesg->mDesiredUsername.contains(QRegExp("\\W"))) {
		reply.mSuccessful = false;
		reply.mMesg = "<br/>The Username Can Only Contain Letters, Numbers, and the Underscore!";
	}
	
	QSqlQuery query;
	query.exec("SELECT email_address, user_name, authenticated FROM plutoite WHERE email_address = '"+mesg->mEmailAddress+"' OR user_name = '" + mesg->mDesiredUsername + "'");
	if(query.size() != 0) {
		reply.mSuccessful = false;
		while(query.next() && (!foundUsername || !foundEmail)) {
			authenticated = query.value(2).toBool();
			if(query.value(0).toString().toLower() == mesg->mEmailAddress.toLower()) {
				foundEmail = true;
			}
			if(query.value(1).toString().toLower() == mesg->mDesiredUsername.toLower()) {
				foundUsername = true;
			}
		}
	}
	/*query.exec("SELECT beta_key, from_email_address FROM beta_invite WHERE to_email_address = '" + mesg->mEmailAddress + "'");
	
	QString fromEmailAddy;
	if(query.next()) {
		hasKey = true;
		if(mesg->mKey == query.value(0).toString()) {
			keyMatches = true;
		}
		fromEmailAddy = query.value(1).toString();
	}*/

	if(authenticated) {
		if(foundEmail) {
			reply.mMesg += "<br/>The Email Address Given is Already in Use.";
		}
		if(foundUsername) {
			reply.mMesg += "<br/>The Username Given is Already in Use.";	
		}
	} else {
		if(foundEmail) {
			query.prepare("DELETE FROM plutoite WHERE email_address = :emailAddress");
			query.bindValue(":emailAddress", mesg->mEmailAddress); 
			query.exec();
		}
		if(foundUsername) {
			query.prepare("DELETE FROM plutoite WHERE user_name = :userName");
			query.bindValue(":userName", mesg->mDesiredUsername);
			query.exec();
		}
	}
	/*if(!hasKey) {
	//	std::cout << "Couldn't find key\n";
		reply.mMesg += "\nThe Email Address You Entered Doesn't Have a Beta Key Associated With It.";
	} else if(!keyMatches) {
	//	std::cout << "Key Invalid!\n";
		reply.mMesg += "\nThe Beta Key You Entered is Invalid";
	}*/
	if(reply.mMesg.isNull() == false) {
		client->sendMessage(&reply);
		return;
	}

	query.prepare("INSERT INTO plutoite(user_guid, email_address, user_name, password, sex, birth_date, first_name, last_name, authentication_key, authenticated)"
			"VALUES(:userID, :emailAddress, :userName, :password, :sex, :birth_date, :firstName, :lastName, :authenticationKey, :authenticated)");
	query.bindValue(":userID", QUuid::createUuid().toString()); //QVariant(QVariant::String));
	query.bindValue(":emailAddress", mesg->mEmailAddress);
	query.bindValue(":firstName", mesg->mFirstName);
	query.bindValue(":lastName", mesg->mLastName);
	query.bindValue(":sex", mesg->mSex);
	query.bindValue(":birth_date", mesg->mBirthDate);
	query.bindValue(":userName", mesg->mDesiredUsername);
	query.bindValue(":password", mesg->mPassword); 
	QString nonce = helperMakeNonce(7);
	query.bindValue(":authentication_key", nonce);
	query.bindValue(":authenticated", false);

	std::cout << "Signing up " << mesg->mPassword.toStdString() << "\n";


	if(query.exec()) {
		client->setEmailAddress(mesg->mEmailAddress);
		reply.mSuccessful = true;
		QString password = helperResetPassword(client);
		helperSendEmail(client->emailAddress(),
				"Welcome to Trek!",
				"Hello " + mesg->mDesiredUsername + ",\n\nThank You for Signing Up for Trek.\n\nPlease use this temporary password to sign in the first time.  You'll be prompted to change the password to something more permanent.\n\nPassword:     " + password + "\n\n--------------------------------------------\nQuestions, Comments?  Just reply to this email and we'll kindly get back to you."); 
	} else {
		reply.mSuccessful = false;
	}
	/*if(query.exec() && QSqlDatabase::database().commit()) {
	//send the successful reply
	reply.mSuccessful = true;
	QProcess p;
	QString program = "/usr/bin/sendEmail";
	QStringList args;
	args << "-f" << "jeremy.kross@gmail.com";
	args << "-t" << mesg->mEmailAddress; 
	args << "-u" << "Welcome to Trek!";
	args << "-m" << mesg->mFirstName + ",\n\nHi there.  Thanks for taking the time to join the Trek community.  In order to verify you are who you say you are, you'll have to enter the following key when you first log into Trek:\n\n Authentication Key:   " + nonce + "\n\nWe hope you enjoy your time on Trek.\n\n----------------------------------------------------\nQuestions, Comments? Just reply to this email and we'll kindly get back to you."; 
	args << "-s" << "smtp.gmail.com:587";
	args << "-xu" << "invite@trekbrowser.com";
	args << "-xp" << "8j12s85k";
	p.start(program, args);
	p.waitForFinished();
	std::cout << QString(p.readAllStandardOutput()).toStdString() << "\n";

	} else {
	//send the unsuccessful reply
	reply.mSuccessful = false;
	}
	 */	
	/*if(fromEmailAddy.isEmpty() == false) {
	  query.prepare("INSERT INTO friendship(follower_id, followee_id) values((SELECT user_id FROM plutoite WHERE email_address=:inviteeEmail), (SELECT user_id FROM plutoite WHERE email_address=:inviterEmail))");
	  query.bindValue(":inviterEmail", fromEmailAddy);
	  query.bindValue(":inviteeEmail", mesg->mEmailAddress); 
	  query.exec();
	  }*/
	client->sendMessage(&reply);
}
void Server::helperSendEmail(const QString& emailAddy, const QString& subject, const QString& message) {
	QProcess p;
	QString program = "/usr/bin/sendEmail";
	QStringList args;
	args << "-f" << "bagman@trekbrowser.com";
	args << "-t" << emailAddy;
	args << "-u" << subject;
	args << "-m" << message;
	args << "-s" << "smtp.gmail.com:587";
                args << "-xu" << "invite@trekbrowser.com";
                args << "-xp" << "8j12s85k";
		std::cout << QProcess::startDetached(program, args) << "\n";
}
void Server::processClientSpeak(ClientData* client, c2sSpeakMessage* mesg) {
	QList<ClientData*> data = mDataByUrl.value(client->currentUrl());
	if(data.count() == 0) return;
	s2cListenMessage reply;
	reply.mGuid = client->guid();
	reply.mCommunique = mesg->mCommunique;
	for(int i=0; i<data.count(); i++) {
		data[i]->sendMessage(&reply);
	}
}
void Server::processClientPersonalSpeak(ClientData* client, c2sPersonalSpeakMessage* mesg) {
	ClientData* recipient = mDataByGuid.value(mesg->mGuid);
	if(recipient == NULL) return;

	s2cPersonalListenMessage reply;
	reply.mGuid = client->guid();
	reply.mCommunique = mesg->mCommunique;
	recipient->sendMessage(&reply);
}

void Server::processClientLocaleChanged(ClientData* client, c2sLocaleChangedMessage* mesg) {
	if(mesg->mOldUrl.isEmpty() == false) {
		mDataByUrl[mesg->mOldUrl].removeOne(client);	
		s2cRemoveUsersMessage informDeparture;
		informDeparture.mUsersCount = 1;
		informDeparture.mGuids.append(client->guid());
		informDeparture.mUrls.append(mesg->mNewUrl);
		for(int i=0; i<mDataByUrl[mesg->mOldUrl].count(); i++) {
			mDataByUrl[mesg->mOldUrl][i]->sendMessage(&informDeparture);
		}
	}
	if(mesg->mNewUrl.isEmpty()) return;
	s2cAddUsersMessage informArrival;		
	informArrival.mUsersCount = 1;
	informArrival.mGuids.append(client->guid());
	informArrival.mUsernames.append(client->username());

	s2cAddUsersMessage reply;
	QList<ClientData*> data = mDataByUrl[mesg->mNewUrl];
	reply.mUsersCount = data.count(); 
	for(int i=0; i<data.count(); i++) {
		reply.mGuids.append(data[i]->guid());
		reply.mUsernames.append(data[i]->username());

		data[i]->sendMessage(&informArrival);
	}
	client->sendMessage(&reply);

	client->setUrl(mesg->mNewUrl);
	client->setMostRecentMove(s2cMoveMessage());
	mDataByUrl[mesg->mNewUrl].append(client);

	if(mExpeditionMap.contains(client)) {
		QMap<ClientData*, bool> party = mExpeditionMap.value(client);
		QMap<ClientData*, bool>::const_iterator i;
		s2cPartyMoveMessage moveMessage;
		moveMessage.mUrl = mesg->mNewUrl; 
		for(i = party.constBegin(); i!=party.constEnd(); ++i) {
			if(i.key() && i.value()) {
				i.key()->sendMessage(&moveMessage);	
			}
		}
	}
}

void Server::processClientChangeProfile(ClientData* client, c2sChangeProfileMessage* mesg) {
	CONNECT_TO_DB;

	QSqlQuery query;
	query.prepare("UPDATE plutoite SET occupational_info = :occupational_info,\
			hobbies = :hobbies,\
			favorite_culture = :favorite_culture,\
			general_info = :general_info\ 
			WHERE user_guid = :guid");
	query.bindValue(":occupational_info", mesg->mOccupationInfo);
	query.bindValue(":hobbies", mesg->mHobbies);
	query.bindValue(":favorite_culture", mesg->mFavoriteCulture);
	query.bindValue(":general_info", mesg->mGeneralInfo);
	query.bindValue(":guid", client->guid().toString());

	if(query.exec() == false) {
		//QMessageBox::information(NULL, "Error", "wtf?");
	}
}
void Server::processClientChangeAppearance(ClientData* client, c2sChangeAppearanceMessage* mesg) {
	CONNECT_TO_DB;

	QSqlQuery query;
	query.prepare("UPDATE plutoite SET username_bg_color = :username_bg_color,\ 
			username_font_color = :username_font_color,\
			balloon_bg_color = :balloon_bg_color,\
			balloon_font_color = :balloon_font_color,\
			icon_file_name = :icon_file_name\
			WHERE user_name = :user_name");

	query.bindValue(":username_bg_color", mesg->mUsernameBGColor.name());
	query.bindValue(":username_font_color", mesg->mUsernameFontColor.name());
	query.bindValue(":balloon_bg_color", mesg->mSpeechBalloonBGColor.name());
	query.bindValue(":balloon_font_color", mesg->mSpeechBalloonFontColor.name());
	query.bindValue(":user_name", client->username());

	QString filename = client->username() + ".jpg";
	query.bindValue(":icon_file_name", filename);

	if(query.exec() && QSqlDatabase::database().commit()) {
		//success
	} else {
		//QMessageBox::information(NULL, "Database Error", QSqlDatabase::database().lastError().text());
	}

	if(mesg->mScaledPixmap.save("avatar_icons/" + filename) == false) { //"C:\\Users\\Jeremy S. Kross\\avatarIcons\\" + client->username() + ".jpg") == false) {
		std::cout << "Failed to Save Image\n";
		//QMessageBox::information(NULL, "Server", "Failed to Save Pixmap");
	}

}

void Server::processClientRequestAppearance(ClientData* client, c2sRequestAppearanceMessage* mesg) {	
	CONNECT_TO_DB;
	QSqlQuery query;

	//query.exec("select username_bg_color, username_font_color, balloon_bg_color, balloon_font_color FROM plutoite WHERE email_address='jeremy.kross@gmail.com'");
	query.prepare("SELECT username_bg_color, username_font_color, balloon_bg_color, balloon_font_color, icon_file_name, user_name FROM plutoite WHERE user_guid = :user_id");
	query.bindValue(":user_id", mesg->mGuid.toString());
	query.exec();
	query.next();

	QImage iconPixmap;
	QColor usernameBGColor, usernameFontColor, balloonBGColor, balloonFontColor;
	QString userName;
	usernameBGColor.setNamedColor(query.value(0).toString());
	usernameFontColor.setNamedColor(query.value(1).toString());
	balloonBGColor.setNamedColor(query.value(2).toString());
	balloonFontColor.setNamedColor(query.value(3).toString());
	iconPixmap.load("avatar_icons/"+query.value(4).toString());
	userName = query.value(5).toString();

	AppearanceMessage reply;
	reply.mGuid = mesg->mGuid;
	reply.hasUsernameBGColor = true;
	reply.hasUsernameFontColor = true;
	reply.hasSpeechBalloonBGColor = true;
	reply.hasSpeechBalloonFontColor = true;
	reply.hasIcon = true;
	reply.hasUsername = true;

	reply.mUsernameBGColor = usernameBGColor;
	reply.mUsernameFontColor = usernameFontColor;
	reply.mSpeechBalloonBGColor = balloonBGColor;
	reply.mSpeechBalloonFontColor = balloonFontColor;
	reply.mIcon = iconPixmap;
	reply.mUsername = userName;

	ClientData* d = mDataByGuid.value(mesg->mGuid);
	if(d) {
		if(d->partyColor().isValid()) {
			reply.hasPartyColor = true;
			reply.mPartyColor = d->partyColor();	
			if(mExpeditionMap.contains(d)) {
				reply.mIsLeader = true;
			}
		}
	}


	client->sendMessage(&reply);
}

void Server::processClientMove(ClientData* client, c2sMoveMessage* mesg) {
	QList<ClientData*> data = mDataByUrl[client->currentUrl()];
	s2cMoveMessage reply;
	reply.mGuid = client->guid();
	reply.mId = mesg->mId;
	reply.mLocationX = mesg->mLocationX;
	reply.mLocationY = mesg->mLocationY;
	reply.mElementX = mesg->mElementX;
	reply.mElementY = mesg->mElementY;
	reply.mElementWidth = mesg->mElementWidth;
	reply.mElementHeight = mesg->mElementHeight;
	reply.mBodyWidth = mesg->mBodyWidth;
	reply.mBodyHeight = mesg->mBodyHeight;

	for(int i=0; i<data.count(); i++) {
		data[i]->sendMessage(&reply);
	}

	client->setMostRecentMove(reply);
}

void Server::processClientFollowUser(ClientData* client, c2sFollowUserMessage* mesg) {
	if(client->guid() == mesg->mFriendGuid.toString()) return;
	CONNECT_TO_DB;
	QSqlQuery query;
	//Insert into friendship 
	query.prepare("SELECT user_id, user_name FROM plutoite WHERE user_guid = :user_guid");
	query.bindValue(":user_guid", mesg->mFriendGuid.toString());
	query.exec();
	query.next();
	unsigned int followeeID = query.value(0).toUInt();
	QString username = query.value(1).toString();

	query.prepare("SELECT user_id, user_name FROM plutoite WHERE user_guid = :user_guid");
	query.bindValue("user_guid", client->guid().toString());
	query.exec();
	query.next();
	unsigned int followerID = query.value(0).toUInt();
	QString username1 = query.value(1).toString();		

	QString currStatus;
	query.prepare("SELECT status FROM friendship WHERE follower_id = :folloower AND followee_id = :followee");
	query.bindValue(":follower", followerID);
	query.bindValue(":followee", followeeID);
	if(query.exec() && query.next()) {
		currStatus = query.value(0).toString();
	}
	bool success = false;	
	if(currStatus.isEmpty()) {
		query.prepare("INSERT INTO friendship(follower_id, followee_id, status)" 
				"VALUES(:follower, :followee, :status)");
		query.bindValue(":follower", followerID);
		query.bindValue(":followee", followeeID);
		query.bindValue(":status", "p");
		success = query.exec(); 
	} else if(currStatus != "v" && currStatus != "p") {
		query.prepare("UPDATE friendship SET status = 'p' WHERE follower_id = :follower AND followee_id = :followee");
		query.bindValue(":follower", followerID);
		query.bindValue(":followee", followeeID);
		success = query.exec();
	}
	if(success) {
		if(mDataById.contains(followeeID)) {
			s2cPendingFollowersMessage mesg;
			mesg.mGuids.append(client->guid());
			mesg.mUsernames.append(client->username());
			mDataById[followeeID]->sendMessage(&mesg);
		}
		s2cPendingFollowingMessage reply;
		reply.mGuids.append(mesg->mFriendGuid);
		reply.mUsernames.append(username);
		client->sendMessage(&reply);
	}

	//Now inform the client that it was a success
	/*
	   QList<QUuid> guids;
	   QList<QString> usernames;
	   guids.append(mesg->mFriendGuid);
	   usernames.append(username);

	   s2cAddFollowedMessage reply;	
	   reply.mOwnerGuid = client->guid();
	   reply.mUsersCount = 1;
	   reply.mGuids = guids;
	   reply.mUsernames = usernames;  
	   client->sendMessage(&reply);
	 */

	/*
	   if(mDataByGuid.contains(mesg->mFriendGuid)) {
	   guids.clear();
	   usernames.clear();
	   guids.append(client->guid());
	   usernames.append(username1);
	   s2cAddFansMessage fanReply;
	   fanReply.mOwnerGuid = mesg->mFriendGuid; 
	   fanReply.mUsersCount = 1;
	   fanReply.mGuids = guids;
	   fanReply.mUsernames = usernames;
	   mDataByGuid[fanReply.mOwnerGuid]->sendMessage(&fanReply);
	   }
	 */
}
void Server::processClientUnfollowUser(ClientData* client, c2sUnfollowUserMessage* mesg) {
	if(client->guid() == mesg->mFriendGuid.toString()) return;
	CONNECT_TO_DB;	
	unsigned int followerId = client->id();

	QSqlQuery query;
	query.prepare("SELECT user_id FROM plutoite WHERE user_guid = :guid");
	query.bindValue(":guid", mesg->mFriendGuid.toString());
	query.exec();
	query.next();
	unsigned int followeeId = query.value(0).toUInt();

	query.prepare("DELETE FROM friendship WHERE follower_id = :follower_id AND followee_id = :followee_id");
	query.bindValue(":follower_id", followerId);
	query.bindValue(":followee_id", followeeId);
	query.exec();
	query.next();

	if(mDataByGuid.contains(mesg->mFriendGuid)) {
		s2cRemoveFansMessage fanReply;
		fanReply.mOwnerGuid = mesg->mFriendGuid; 
		fanReply.mUsersCount = 1; 
		fanReply.mGuids.append(client->guid());
		mDataByGuid[fanReply.mOwnerGuid]->sendMessage(&fanReply);	
	}
}	


void Server::informRelationsOfStatusChange(const QUuid& guid, Status status) {
	CONNECT_TO_DB;
	QSqlQuery query;
	/*query.prepare("SELECT user_id FROM plutoite WHERE guid = :guid");
	  query.bindValue(":guid", guid.toString());
	  query.exec();
	  query.next(); */
	if(mDataByGuid.contains(guid) == false) return;

	unsigned int userID = mDataByGuid[guid]->id(); //query.value(0).toUInt();

	query.prepare("SELECT follower_id FROM friendship WHERE followee_id = :followee_id");
	query.bindValue(":followee_id", userID);
	query.exec();

	QList<unsigned int> followerIds;
	while(query.next() && query.isValid()) {
		followerIds.append(query.value(0).toUInt());	
	}
	query.prepare("SELECT followee_id FROM friendship WHERE follower_id = :followee_id");
	query.bindValue(":follower_id", userID);
	query.exec();

	while(query.next() && query.isValid()) {
		followerIds.append(query.value(0).toUInt());	
	}

	s2cStatusMessage info;
	ClientData* client = NULL;
	for(int i=0; i<followerIds.count(); i++) {
		if(mDataById.contains(followerIds[i])) {
			client = mDataById[followerIds[i]];
			info.mGuid = guid;
			info.mStatus = status;
			client->sendMessage(&info);
		}
	}

}

void Server::informPartyOfArrival(ClientData* client) {
	if(mExpeditionMap.contains(client->leader())) {
		QMap<ClientData*, bool> party = mExpeditionMap.value(client->leader());
		if(party.contains(client)) {
			s2cPartyAddUsersMessage mesg;
			mesg.mUsersCount = 1;
			mesg.mGuids.append(client->guid());
			mesg.mUsernames.append(client->username());
			client->leader()->sendMessage(&mesg);
			QMap<ClientData*, bool>::const_iterator i;
			for(i=party.constBegin(); i!=party.constEnd(); i++) {
				if(i.value() && i.key()) {
					if(i.key() != client)
						i.key()->sendMessage(&mesg);	
				}
			}
		}
	}
}

void Server::informPartyOfDeparture(ClientData* client) {
	std::cout << "Informing Departure\n";
	ClientData* leader = NULL;
	if(mExpeditionMap.contains(client)) {
		leader = client;
	} else if(mExpeditionMap.contains(client->leader())) { 
		leader = client->leader();	
	}
	if(leader == NULL) return;

	QMap<ClientData*, bool > party = mExpeditionMap.value(leader);
	QList<QUuid> partyGuids;
	//	if(party.contains(client)) {
	QMap<ClientData*, bool>::const_iterator i;
	s2cPartyRemoveUsersMessage mesg;
	mesg.mUsersCount=1;
	mesg.mGuids.append(client->guid());
	for(i=party.constBegin(); i!=party.constEnd(); i++) {
		if(i.value() && i.key()) {
			if(i.key() != client)
				i.key()->sendMessage(&mesg);	
			if(client == leader) {
				i.key()->setLeader(NULL);
				partyGuids.append(i.key()->guid());
			}
		}
	}
	AppearanceMessage appMesg;
	appMesg.hasPartyColor = true;
	appMesg.mPartyColor = QColor();
	if(leader != client) {
		client->leader()->sendMessage(&mesg);
		party.remove(client);
		mExpeditionMap.insert(client->leader(), party);
		client->setLeader(NULL);
		client->setPartyColor(QColor());
		appMesg.mGuid = client->guid();
		sendToUrl(client->currentUrl(), &appMesg);
	} else {
		mExpeditionMap.remove(leader);
		for(int i=0; i<partyGuids.count(); i++) {
			appMesg.mGuid = partyGuids[i];
			mDataByGuid.value(partyGuids[i])->setPartyColor(QColor());
			mDataByGuid.value(partyGuids[i])->setLeader(NULL);
			sendToUrl(mDataByGuid[partyGuids[i]]->currentUrl(), &appMesg);
		}
		appMesg.mGuid = leader->guid();
		sendToUrl(leader->currentUrl(), &appMesg);
		leader->setPartyColor(QColor());
	}
}
