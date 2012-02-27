#include "MainWindow.h"

MainWindow::MainWindow() {
	userData = NULL;
	mFollowedModel  = NULL;
	mFansModel      = NULL;
	mBookmarkModel  = NULL;
	mHistoryModel   = NULL;
	mLoginWidget    = NULL;
	userListPanel   = NULL;
	mExpeditionManager = NULL;
        mPartyLeader = NULL;
	//mDonationMenu = NULL;
	mExpeditionMenu = NULL;
	mPageOptionsMenu = NULL;
	mConfigOptionsMenu = NULL;
        mIsInParty = false;
	webView = NULL;
	graphicsView = NULL;
	graphicsScene = NULL;
	browsingWidget = NULL;
	userAvatar = NULL;

	initialize();

	QApplication* currApp = (QApplication*)QApplication::instance();
	if(currApp) {
		connect(currApp, SIGNAL(focusChanged(QWidget*, QWidget*)), this, SLOT(focusChanged(QWidget*, QWidget*)));
	}
}
MainWindow::~MainWindow() {
	//Destructor
	int tabCount = tabWidget.count();
	for(int i=0; i<tabCount; i++) {
		closeTab();
	}
	if(webView)
		webView->stop();
//	if(mDonationMenu) { delete mDonationMenu; mDonationMenu = NULL; }
	if(mExpeditionMenu) { delete mExpeditionMenu; mExpeditionMenu = NULL;}
	if(mPageOptionsMenu) {delete mPageOptionsMenu; mPageOptionsMenu = NULL;}
	if(mConfigOptionsMenu) {delete mConfigOptionsMenu; mConfigOptionsMenu = NULL;}

	/*if(browsingWidget) {
		browsingWidget->deleteLater();
	}*/

	QMap<QUuid, PersonalMessageWidget*>::iterator it;
	for(it=mCurrentChats.begin(); it != mCurrentChats.end(); ++it) {
		it.value()->deleteLater();
	}

	if(mExpeditionManager) {
		mExpeditionManager->close();
		mExpeditionManager->deleteLater();
	}

	if(userAvatar) {
		delete userAvatar;
	}
}

/* Tutorial Methods */
// enableTutorial() sets up the signal/slots to run the tutorial.
//reenableTutorial calls enableTutorial(), and resets the settings indicating how much of the tutorial has already been viewed.
void MainWindow::reenableTutorial() {
	QSettings settings("Kukundu Soft", "Trek");
	settings.beginGroup(userData->displayName());
	settings.remove("");
	settings.setValue("initial_login_complete", true);
	settings.setValue("tutorial/base_done", true);
	settings.endGroup();

	enableTutorial();
	showTutorial();
}

void MainWindow::enableTutorial() {
	QSettings settings("Kukundu Soft", "Trek");
	settings.beginGroup(userData->displayName());
	if(settings.value("tutorial/all_done").toBool() == false) { 
		connect(panelsDock, SIGNAL(panelOpened(PanelWidget*)), this, SLOT(panelOpened(PanelWidget*)));
		connect(mTickerWidget, SIGNAL(done()), this, SLOT(helperShowAllPanels()));
	}
	if(settings.value("tutorial/base_done").toBool() == false) {
		connect(&mCentralWidget, SIGNAL(done()), this, SLOT(showTutorial()));
	}
	settings.endGroup();
}

void MainWindow::informDownloadsDisabled() {
	QMessageBox::information(this, "Sorry", "Downloads are disabled in this build.");
}


void MainWindow::prepare() {
	mLoginWidget = new LoginFeedbackWidget();
	QObject::connect(mLoginWidget, SIGNAL(allowed(UserData*)), this, SLOT(begin(UserData*)));

	mLoginWidget->show();
}

void MainWindow::initialize() {
	browsingWidget = NULL;
	graphicsView = NULL;
	graphicsScene = NULL;
	webView = NULL;


	setWindowTitle(WINDOW_TITLE);	
	setWindowIcon(QIcon((QPixmap(PLUTO_ICON).scaled(64,64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))));
	
	connect(Controller::self(), SIGNAL(showSystemTrayMessage(const QString&, const QString&)), this, SLOT(controller_showSystemTrayMessage(const QString&, const QString&)));

	setupActions();
	setupToolBar();
	setupMenus();
	setupCentralWidget();
	setupDocks();
	setupPanels();

	connect(Controller::self(), SIGNAL(avatarAdded(UserData*)), this, SLOT(addAvatar(UserData*)));

	connect(chatWidget, SIGNAL(messageSent(const QString&)), Controller::self(), SIGNAL(serverSpeak(const QString&)));
	connect(Controller::self(), SIGNAL(communiqueReceived(const QUuid&, const QString&)), this, SLOT(communiqueReceived(const QUuid&, const QString&)));
	connect(Controller::self(), SIGNAL(avatarPositionChanged(const QUuid&, unsigned int, float, float)), this, SLOT(avatarPositionChanged(const QUuid&, unsigned int, float, float)));
	connect(Controller::self(), SIGNAL(avatarPositionChangedBackup(const QUuid&, float, float)), this, SLOT(avatarPositionChangedBackup(const QUuid&, float, float)));

	connect(Controller::self(), SIGNAL(serverConnectionLost()), this, SLOT(connectionLost()));

	connect(this, SIGNAL(localeChanged(QUrl, QUrl)), Controller::self(), SIGNAL(serverChangeLocale(QUrl, QUrl)));
	connect(this, SIGNAL(positionChanged(unsigned int, int,int,int,int,int,int, int, int)), Controller::self(), SIGNAL(serverChangePosition(unsigned int, int,int,int,int,int,int,int,int)));
	connect(this, SIGNAL(downloadRequested(const QNetworkRequest&)), Controller::self(), SIGNAL(downloadRequested(const QNetworkRequest&)));
        connect(Controller::self(), SIGNAL(downloadStarted(DownloadData*)), this, SLOT(downloadStarted(DownloadData*)));

	connect(this, SIGNAL(appearanceRequested(const QUuid&, bool, bool, bool, bool, bool, bool)), Controller::self(), SIGNAL(serverRequestAppearance(const QUuid&, bool, bool, bool, bool, bool, bool)));
#ifdef ADMIN
//	connect(this, SIGNAL(executeAdminCommand(const QString&, int)), Controller::self(), SIGNAL(serverExecuteAdminCommand(const QString&, int)));
#endif
	connect(this, SIGNAL(followUser(const QUuid&)), Controller::self(), SIGNAL(serverRequestFollow(const QUuid&)));
	connect(this, SIGNAL(unfollowUser(const QUuid&)), Controller::self(), SIGNAL(serverRequestUnfollow(const QUuid&)));
	connect(this, SIGNAL(blockUser(const QUuid&)), Controller::self(), SIGNAL(serverBlock(const QUuid&)));
	connect(this, SIGNAL(controller_serverRequestPendingFollowers()), Controller::self(), SIGNAL(serverRequestPendingFollowers()));
	connect(this, SIGNAL(controller_serverRequestPendingFollowing()), Controller::self(), SIGNAL(serverRequestPendingFollowing()));

	connect(Controller::self(), SIGNAL(avatarAppearanceChanged(const QUuid&, const QPixmap&, const QString&, const QColor&, const QColor&, const QColor&, const QColor&)), this, SLOT(changeAvatarAppearance(const QUuid&, const QPixmap&, const QString&, const QColor&, const QColor&, const QColor&, const QColor&)));
	connect(Controller::self(), SIGNAL(avatarPartyAppearanceChanged(const QUuid&, const QColor&, bool)), this, SLOT(changeAvatarPartyAppearance(const QUuid&, const QColor&, bool)));
	connect(Controller::self(), SIGNAL(avatarRemoved(const QUuid&, const QUrl&)), this, SLOT(removeAvatar(const QUuid&, const QUrl&)));

	connect(Controller::self(), SIGNAL(userContextMenuRequested(UserData*)), this, SLOT(showUserContextMenu(UserData*)));
	connect(Controller::self(), SIGNAL(userDoubleClicked(UserData*)), this, SLOT(showProfile(UserData*)));
	connect(Controller::self(), SIGNAL(askingExpeditionJoin(const QUuid&)), this, SLOT(controller_askingExpeditionJoin(const QUuid&)));
	connect(Controller::self(), SIGNAL(partyAddUser(UserData*)), this, SLOT(controller_partyAddUser(UserData*)));
	connect(Controller::self(), SIGNAL(partyRemoveUser(const QUuid&)), this, SLOT(controller_partyRemoveUser(const QUuid&)));
	connect(Controller::self(), SIGNAL(partyEjected()), this, SLOT(controller_partyEjected()));
	
	connect(Controller::self(), SIGNAL(userOnline(UserData*)), this, SLOT(controller_userOnline(UserData*)));
	connect(Controller::self(), SIGNAL(userOffline(UserData*)), this, SLOT(controller_userOffline(UserData*)));
		
	connect(this, SIGNAL(controller_logHistory(const QString&, const QUrl&)), Controller::self(), SIGNAL(logHistory(const QString&, const QUrl&)));
	connect(this, SIGNAL(controller_addFavorite(const QString&, const QUrl&)), Controller::self(), SIGNAL(addFavorite(const QString&, const QUrl&))); 
	connect(this, SIGNAL(controller_removeFavorite(const QString&, const QUrl&)), Controller::self(), SIGNAL(removeFavorite(const QString&, const QUrl&)));
//	connect(this, SIGNAL(controller_toggleFavorite(const QString&, const QUrl&)), Controller::self(), SIGNAL(toggleFavorite(const QString&, const QUrl&)));

	//connect(this, SIGNAL(controller_heartSetRequested(const QUrl&)), Controller::self(), SIGNAL(heartSetRequested(const QUrl&)));
//	connect(Controller::self(), SIGNAL(heartSetReceived(const QUrl&, bool)), this, SLOT(setHeart(const QUrl&, bool)));

	connect(Controller::self(), SIGNAL(personalCommuniqueReceived(const QUuid&, const QString&)), this, SLOT(privateChatRequested(const QUuid&, const QString&)));

	connect(this, SIGNAL(controller_fansModelRequested(const QUuid&)), Controller::self(), SIGNAL(fansModelRequested(const QUuid&)));
	connect(this, SIGNAL(controller_followedModelRequested(const QUuid&)), Controller::self(), SIGNAL(followedModelRequested(const QUuid&)));
	//connect(this, SIGNAL(controller_bookmarkModelRequested()), Controller::self(), SIGNAL(bookmarkModelRequested()));
	connect(this, SIGNAL(controller_historyModelRequested(const QUuid&)), Controller::self(), SIGNAL(historyModelRequested(const QUuid&)));
	connect(this, SIGNAL(controller_favoritesModelRequested(const QUuid&)), Controller::self(), SIGNAL(favoritesModelRequested(const QUuid&)));

	connect(this, SIGNAL(controller_serverRequestLocations()), Controller::self(), SIGNAL(serverRequestLocations()));
	connect(this, SIGNAL(controller_changePersonalMotd(const QString&)), Controller::self(), SIGNAL(serverChangePersonalMotd(const QString&)));
	connect(this, SIGNAL(controller_serverClearHistory()), Controller::self(), SIGNAL(serverClearHistory()));


	connect(this, SIGNAL(controller_serverExpeditionRequestResponse(const QUuid&, bool)), Controller::self(), SIGNAL(serverExpeditionRequestResponse(const QUuid&, bool)));

	connect(Controller::self(), SIGNAL(fansModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_fansModelOffered(const QUuid&, RelationshipListModel*)));
	connect(Controller::self(), SIGNAL(followedModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_followedModelOffered(const QUuid&, RelationshipListModel*)));
	connect(Controller::self(), SIGNAL(favoritesModelReceived(const QUuid&, SiteListItemModel*)), this, SLOT(controller_favoritesModelReceived(const QUuid&, SiteListItemModel*)));
	connect(Controller::self(), SIGNAL(historyModelReceived(const QUuid&, SiteListItemModel*)), this, SLOT(controller_historyModelReceived(const QUuid&, SiteListItemModel*)));

	connect(Controller::self(), SIGNAL(linkActivated(const QUrl&)), this, SLOT(navigate(const QUrl&)));
	connect(Controller::self(), SIGNAL(linkActivated(const QString&)), this, SLOT(navigate(const QString&)));

	connect(Controller::self(), SIGNAL(partyMove(const QUrl&)), this, SLOT(partyNavigate(const QUrl&)));

	connect(Controller::self(), SIGNAL(serverInviteToExpedition(const QColor&, QList<UserData*>)), this, SLOT(expeditionStartAsLeader(const QColor&)));

	QDir dir(QDir::homePath());
	dir.mkdir(".trek");
	QWebSettings::setIconDatabasePath(dir.path()+"/.trek/");

	newTab();
	
	graphicsScene = browsingWidget->graphicsScene();
	graphicsView = browsingWidget->graphicsView(); 
	webView = browsingWidget->webView();
}

void MainWindow::begin(UserData* myUserData) {
	//Taking ownership of userData
	userData = myUserData;
	usersWidget.addUser(userData);
	
	bookmarkWidget.setOwner(userData);
        historyWidget.setOwner(userData);
	relationshipWidget.setOwner(userData);
	mPendingFollowerWidget->setOwner(userData);
	//relationshipWidget.setViewFans();

	//Constructing userAvatar and speechBalloon on head
        userAvatar = new GraphicsAvatarItem(userData);
        new GraphicsSpeechBalloonItem(userAvatar, userData);

	emit appearanceRequested(userData->guid(), true, true, true, true, true, true);

	mLoginWidget->close();

	navigate("http://www.google.com");

	mCentralWidget.setNextWidget(&tabWidget,new ZoomTransition(1500));
	connect(mIntroWidget, SIGNAL(done()), &mCentralWidget, SLOT(go()));
	connect(mIntroWidget, SIGNAL(done()), mIntroWidget, SLOT(deleteLater()));

	emit controller_fansModelRequested(userData->guid());
	emit controller_followedModelRequested(userData->guid());
	emit controller_favoritesModelRequested(userData->guid());
	emit controller_historyModelRequested(userData->guid());

	emit controller_serverRequestPendingFollowers();
	emit controller_serverRequestPendingFollowing();

	emit ready();

	/*QMessageBox* m = new QMessageBox(QMessageBox::Information, QString("Welcome!"), QString(INTRO_MESG), QMessageBox::Ok, this, Qt::Sheet|Qt::Dialog);
	m->setWindowModality(Qt::WindowModal);
	m->exec(); 
	m->deleteLater();
	m=NULL;*/

	GIMME_SETTINGS
	if(settings.value("initial_login_complete").toBool() == false) { 
		settings.setValue("initial_login_complete", true);
		QMessageBox* m = new QMessageBox(QMessageBox::Question, QString("View Tutorial?"), "Would you like to enable a brief tutorial (highly recommended)?", QMessageBox::Yes|QMessageBox::No, this, Qt::Sheet|Qt::Dialog);
		m->setWindowModality(Qt::WindowModal);
		int response = m->exec(); 
		m->deleteLater();
		m=NULL;
		if(response == QMessageBox::Yes) {
			enableTutorial();
		}
	}

	mGoIncognitoCheck->setChecked(settings.value("isIncognito").toBool());
	SETTINGS_DONE

	mIntroWidget->start(500);

	mLoginWidget->deleteLater();
	mLoginWidget  = NULL;

	if(mTickerWidget->isShowingMessage() == false) {
		mTickerWidget->hide();
	}

	connect(&mCentralWidget, SIGNAL(done()), this, SLOT(begun()));
}

void MainWindow::begun() {
	GIMME_SETTINGS
	if(settings.contains("logHidden")) {
		hideChatLog(settings.value("logHidden").toBool());
	}
	SETTINGS_DONE
}

void MainWindow::controller_fansModelOffered(const QUuid& guid, RelationshipListModel* mModel) {
	if(guid != userData->guid()) return;
	mFansModel = mModel;
	disconnect(Controller::self(), SIGNAL(fansModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_fansModelOffered(const QUuid&, RelationshipListModel*)));
}
void MainWindow::controller_followedModelOffered(const QUuid& guid, RelationshipListModel* mModel) {
	if(guid != userData->guid()) return;
	mFollowedModel = mModel;
	disconnect(Controller::self(), SIGNAL(followedModelOffered(const QUuid&, RelationshipListModel*)), this, SLOT(controller_followedModelOffered(const QUuid&, RelationshipListModel*)));
}

void MainWindow::controller_historyModelReceived(const QUuid& guid, SiteListItemModel* model) {
	if(guid != userData->guid() || mHistoryModel != NULL) return;
	mHistoryModel = model;
	disconnect(Controller::self(), SIGNAL(historyModelReceived(const QUuid&, SiteListItemModel*)), this, SLOT(controller_historyModelReceived(const QUuid&, SiteListItemModel*)));
}
void MainWindow::controller_favoritesModelReceived(const QUuid& guid, SiteListItemModel* model) {
	if(guid != userData->guid() || mBookmarkModel != NULL) return;
	mBookmarkModel = model;
	disconnect(Controller::self(), SIGNAL(favoritesModelReceived(const QUuid&, SiteListItemModel*)), this, SLOT(controller_favoritesModelReceived(const QUuid&, SiteListItemModel*)));
}

void MainWindow::closeEvent(QCloseEvent*) {
        //QMessageBox::about(this,QString("Info"), QString("Close intercepted!"));
	QApplication::exit(0);

}
void MainWindow::keyReleaseEvent(QKeyEvent* evt) {
	if(evt->key() == Qt::Key_Escape) {
                chatWidget->lineEdit()->setFocus(Qt::OtherFocusReason);
	}
}
void MainWindow::paintEvent(QPaintEvent*) {}

void MainWindow::communiqueReceived(const QUuid& guid, const QString& mesg) {
	if(guid == userData->guid()) {
		userAvatar->speak(mesg);
		chatWidget->logMessage(userData, mesg);
		QSound::play("sounds/speak.wav");
	} else {
		if(contempAvatars.contains(guid)) {
			contempAvatars[guid]->speak(mesg);
			chatWidget->logMessage(contempData[guid], mesg);
		}
	}
}

void MainWindow::avatarPositionChanged(const QUuid& guid, unsigned int id, float percentX, float percentY) {
	//QMessageBox::information(NULL, "MainWindow", "positionChanged");
	//QMessageBox::information(NULL, "MainWindow", QString("%1, %2").arg(percentX).arg(percentY));
	if(guid == userData->guid()) return;
	NodeInfo n = browsingWidget->jsDomNode(id);
	if(n.mId == -1 || contempAvatars.contains(guid) == false) {
		//The node could not be found;
		contempAvatars[guid]->setLost(true);
		return;
	}
	int posX = n.mX + n.mWidth*percentX;
	int posY = n.mY + n.mHeight*percentY;
	if(posX <= 0 || posY <= 0) {
		//contempAvatars[guid]->moveTo(NEWLY_ADDED_X, NEWLY_ADDED_Y);
		contempAvatars[guid]->setLost(true);
	} else {
		contempAvatars[guid]->setLost(false);
        	contempAvatars[guid]->moveTo(posX, posY);	
	}
}
void MainWindow::avatarPositionChangedBackup(const QUuid& guid, float percentX, float percentY) {
	if(guid == userData->guid()) return;
	if(contempAvatars[guid] && contempAvatars[guid]->isLost() == false) return;
	if(webView == NULL) return;
	if(webView->page() == NULL) return;
	contempAvatars[guid]->moveTo(webView->page()->mainFrame()->contentsSize().width()*percentX, webView->page()->mainFrame()->contentsSize().height()*percentY);
}

void MainWindow::removeAvatar(const QUuid& guid, const QUrl& newUrl) {
	GraphicsAvatarItem* avatar = NULL;
	if(contempAvatars.contains(guid)) {
		avatar = contempAvatars[guid];
		contempAvatars.remove(guid);
	}
	UserData* data = NULL;
	if(contempData.contains(guid)) {
		data = contempData[guid];
		contempData.remove(guid);
	}
	usersWidget.removeUser(data);
	if(avatar)
		avatar->animatedHide();
	if(data)
		chatWidget->logExit(data, newUrl);
	
}
void MainWindow::addAvatar(UserData* userData) {
	GraphicsAvatarItem* newAvatar = new GraphicsAvatarItem(userData);
        new GraphicsSpeechBalloonItem(newAvatar, userData);
	newAvatar->setPos(NEWLY_ADDED_X+rand()%50, NEWLY_ADDED_Y+rand()%50);
	graphicsScene->addItem(newAvatar);
	usersWidget.addUser(userData);

	contempAvatars.insert(userData->guid(), newAvatar);	
	contempData.insert(userData->guid(), userData);
	emit appearanceRequested(userData->guid(), true, true, true, true, true, true);

	chatWidget->logEntrance(userData);
}

void MainWindow::changeAvatarPartyAppearance(const QUuid& guid, const QColor& color, bool isLeader) {
	//really just need the last else if here.
	UserData* data;
	if(userData == NULL) return;
	if(guid == userData->guid()) data = userData;
	else if(contempData.contains(guid)) data = contempData[guid];
	else if(UserDataFactory::self()->isUserDataKnown(guid)) data = UserDataFactory::self()->userDataForGuid(guid);
	else return;

	data->setPartyColor(color, isLeader);
//	data->altered();
}

void MainWindow::changeAvatarAppearance(const QUuid& guid, const QPixmap& pixmap, const QString& username, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& balloonFontColor) {
	UserData* data;
	if(userData == NULL) return;
	if(guid == userData->guid()) data = userData;
	else if(contempData.contains(guid)) data = contempData[guid];
	else return;


	if(pixmap.isNull() == false)
		data->setIcon(pixmap);
	if(username.isEmpty() == false) 
		data->setDisplayName(username);
	if(usernameBGColor.isValid()) 
		data->setUsernameBGColor(usernameBGColor);
	if(usernameFontColor.isValid())
		data->setUsernameFontColor(usernameFontColor);
	if(balloonBGColor.isValid()) 
		data->setBalloonBGColor(balloonBGColor);
	if(balloonFontColor.isValid())
		data->setBalloonFontColor(balloonFontColor);

	data->altered();
}

void MainWindow::changeLocalUserData(UserData*) {}

void MainWindow::setAvatarOpacity(int opacity) {
	if(mToggleAvatarsAction && graphicsView) {
		mToggleAvatarsAction->setChecked(false);
		GraphicsAvatarItem::setGlobalOpacity(((float)opacity/100));
		graphicsView->update();
	}
}	
/*void MainWindow::resetScale() {
	mScaleSlider->setValue(25);
}*/
/*void MainWindow::setAvatarScale(int scale) {
	GraphicsAvatarItem::setGlobalScale(.5 + ((float)scale/100)*2);
	if(graphicsView)
		graphicsView->update();
}*/
void MainWindow::toggleFollowed(UserData* d) {
	if(!mFollowedModel) return;
	
	if(d == userData) { 
		QMessageBox::information(this, "Sorry", "You can't be your own fan.");
		return;
	}

	if(mFollowedModel->contains(d)) {
		/*QMessageBox::StandardButton response = QMessageBox::question(this, "Unfan User?", "Are you sure you want to stop being " + d->displayName() +" fan?", QMessageBox::Yes|QMessageBox::No);
		if(response == QMessageBox::Yes) {
			emit unfollowUser(d->guid());
			mFollowedModel->removeUser(d);
		}*/
		QMessageBox::information(this, "What Luck!", "You're already one of " + d->displayName() + "'s fans.");
	} else {
		QMessageBox::StandardButton response = QMessageBox::question(this, "Become a Fan?", "Are you sure you want to send a Fan request to " + d->displayName()+ "?", QMessageBox::Yes|QMessageBox::No);
		if(response == QMessageBox::Yes) {
			if(d != userData)
				emit followUser(d->guid());
		}

	}
}
void MainWindow::followTriggered() {
	QAction* action = ((QAction*)sender());
	QVariant dataAsVariant = action->data();
	UserData* data = dataAsVariant.value<UserData*>();
	if(data->guid() != userData->guid())  
		emit followUser(data->guid());
}
void MainWindow::unfollowTriggered() {
	QAction* action = ((QAction*)sender());
	QVariant dataAsVariant = action->data();
	UserData* data = dataAsVariant.value<UserData*>();
	if(data->guid() != userData->guid())  
		emit unfollowUser(data->guid());
	mFollowedModel->removeUser(data);	
}
void MainWindow::blockTriggered() {
	QAction* action = ((QAction*)sender());
	QVariant dataAsVariant = action->data();
	UserData* data = dataAsVariant.value<UserData*>();
	if(data->guid() != userData->guid()) { 
		emit blockUser(data->guid());
	}
	mFansModel->removeUser(data);
}
void MainWindow::chatTriggered() {
	QAction* action = ((QAction*)sender());
	QVariant dataAsVariant = action->data();
	UserData* data = dataAsVariant.value<UserData*>();
	PersonalMessageWidget* pmw = personalChat(data);
	pmw->show();
}
void MainWindow::showTutorial() {

	helperHideAllPanelsBut(NULL);
	controller_showSystemTrayMessage("", "Hello there!  Your attention please!");
	controller_showSystemTrayMessage("", "This is the message ticker (optionally click to advance).");
	controller_showSystemTrayMessage("", "The message ticker informs you of relevant events within the Trek ecosystem.");
	controller_showSystemTrayMessage("", "When you get a message, if you're not sure where to address it...");
	controller_showSystemTrayMessage("", "..clicking on the ticker will open the relevant panel (greyed icons to the left).");
	helperShowMessages(QStringList() << "Most of Trek's functionality is contained within these panels." << "Because this is your first login, opening each panel will offer a brief description." << "Thanks and enjoy your stay!"); 
}
void MainWindow::hideChatLog(bool value) {
	if(chatWidget) {
		chatWidget->setLogVisible(!value);	
		mToggleChatLogAction->setChecked(value);
	}

	GIMME_SETTINGS
	settings.setValue("logHidden", value);
	SETTINGS_DONE
}
void MainWindow::dataWidgetClosed(UserDataWidget* w) {
	mCurrentDataWidgets.remove(w->userData()->guid());
	delete w;
}

UserDataWidget* MainWindow::readyView(UserData* data) {
	UserDataWidget* w;
	if(mCurrentDataWidgets.contains(data->guid())) {
		w = mCurrentDataWidgets[data->guid()];
	} else {
		w = new UserDataWidget();
		w->setUserData(data);	
		mCurrentDataWidgets.insert(data->guid(), w);
		connect(w, SIGNAL(closed(UserDataWidget*)), this, SLOT(dataWidgetClosed(UserDataWidget*)));
	}
	w->show();
	return w;
}

UserDataWidget* MainWindow::readyView(QAction* action) {
	QVariant dataAsVariant = action->data();
	UserData* data = dataAsVariant.value<UserData*>();
	return readyView(data);
}
void MainWindow::viewFollowedTriggered() {
	readyView((QAction*)sender())->showFollowed();
}
void MainWindow::viewFansTriggered() {
	readyView((QAction*)sender())->showFans();
}
void MainWindow::viewProfileTriggered() {
	readyView((QAction*)sender())->showProfile();
}
void MainWindow::viewFavoritesTriggered() {
	readyView((QAction*)sender())->showFavorites();
}
void MainWindow::viewHistoryTriggered() {
	readyView((QAction*)sender())->showHistory();
}
void MainWindow::linkInNewTabTriggered() {
	QAction* action = ((QAction*)sender());
	QVariant dataAsVariant = action ->data();
	QUrl url = dataAsVariant.toUrl();
	newTab(url);
}
void MainWindow::viewFriendsTriggered() {
	QAction* action = ((QAction*)sender());
	QVariant dataAsVariant = action->data();
	UserData* data = dataAsVariant.value<UserData*>();

	UserDataWidget* w = new UserDataWidget();
	w->setUserData(data);	
	w->show();
	//w->showFriends();
}

void MainWindow::privateChatRequested(const QUuid& guid, const QString& mesg) {
	UserData* d = UserDataFactory::self()->userDataForGuid(guid);
	PersonalMessageWidget* pmw = NULL;
	if(mCurrentChats.contains(guid) == false) {
		pmw = personalChat(d); 
		pmw->logMessage(d, mesg);
		pmw->show();
	} else {
		pmw = personalChat(d);	
	}
	if(pmw->isVisible() == false) {
		//QString truncated = mesg;
		//truncated.truncate(19);
		//if(mesg.count() > 19)
		//	truncated.append("...");
		controller_showSystemTrayMessage("PM from " + d->displayName(), ": "+mesg);
	}
	//else
	//	pmw->setFocus(Qt::OtherFocusReason);
	
}

void MainWindow::controller_partyAddUser(UserData* data) {
	if(data == userData) return;
	if(mPartyLeader == NULL) {
		mPartyLeader = data;
	} else if(mPartyLeader == userData) {
		controller_showSystemTrayMessage(data->displayName(), "Has Joined Your Browsing Trek.");	
	}
	mPartyData.append(data);
//	usersWidget.partyUserWidget()->addUser(data);
}
void MainWindow::controller_partyRemoveUser(const QUuid& guid) {
	if(mPartyLeader && guid == mPartyLeader->guid()) {
		controller_showSystemTrayMessage(mPartyLeader->displayName(), "Has Disbanded the Trek.");	
		mIsInParty = false;
		mPartyLeader = NULL;
		mPartyData.clear();
		//usersWidget.partyUserWidget()->removeAllUsers();
		setupExpeditionMenu();
	} else {
		UserData* data = UserDataFactory::self()->userDataForGuid(guid);
		if(data) {
			mPartyData.removeAll(data);
			setupExpeditionMenu();
			//usersWidget.partyUserWidget()->removeUser(data);
		}
		if(mPartyLeader == userData) {
			controller_showSystemTrayMessage(data->displayName(), "Has Left Your Browsing Trek.");	
		}
	}
}

void MainWindow::controller_partyEjected() {
	controller_showSystemTrayMessage("You", "Have Been Kicked Out of the Trek (something you said, perhaps?)");
	mIsInParty = false;
	mPartyLeader = NULL;
	mPartyData.clear();
	//usersWidget.partyUserWidget()->removeAllUsers();
	setupExpeditionMenu();

}

void MainWindow::controller_askingExpeditionJoin(const QUuid& guid) {
	UserData* d = UserDataFactory::self()->userDataForGuid(guid);
	controller_showSystemTrayMessage(d->displayName(), "Would Like You to Join a Browsing Trek!");
	messageCenterWidget.logExpeditionRequest(d);
}

void MainWindow::expeditionStartAsLeader(const QColor&) {
	mPartyLeader = userData;
	//mPartyData.append(userData);	
	//usersWidget.partyUserWidget()->addUser(userData);	
	setupExpeditionMenu();

/*	if(userData) {
		userData->setPartyColor(c, true);
	} */
}

void MainWindow::expeditionAsk(UserData* d) {
	QMessageBox::StandardButton response = QMessageBox::question(this, "Join Trek?", "Join " + d->displayName() + "'s Browsing Party?", QMessageBox::Yes|QMessageBox::No);
	if(mPartyLeader != NULL && response == QMessageBox::Yes) {
		QMessageBox::StandardButton response1 = QMessageBox::question(this, "Continue?", "You are already on a Trek.  Continuing will remove you from your current browsing party.  Still Join?", QMessageBox::Yes|QMessageBox::No);
		response = response1;
	}	
	if(response == QMessageBox::Yes) {
		emit controller_serverExpeditionRequestResponse(d->guid(), true);
		messageCenterWidget.logExpeditionRequestResponse(d, true);	
		mIsInParty = true;
		mPartyLeader = d;
		//usersWidget.partyUserWidget()->addUser(userData);
		setupExpeditionMenu();
		
		int tabCount = tabWidget.count();
		for(int i=0; i<tabCount; i++) {
			closeTab();
		}
		
	} else {
		messageCenterWidget.logExpeditionRequestResponse(d, false);	
		emit controller_serverExpeditionRequestResponse(d->guid(), false);
	}
	
}

void MainWindow::resetCorrectOpacity() {
	if(mToggleAvatarsAction && mOpacitySlider) {
		if(mToggleAvatarsAction->isChecked() == false) {
			setAvatarOpacity(mOpacitySlider->value());
		}
	}
}
/*void MainWindow::resetCorrectScale() {
	setAvatarScale(mScaleSlider->value());
}*/

void MainWindow::partyNavigate(const QUrl& url) {
	//((AddressComboBox*)addressBar)->addressLineEdit()->setReadOnly(true);
	bool wasInParty =mIsInParty;
	mIsInParty = false;
	navigate(url);
	mIsInParty = wasInParty;
}
void MainWindow::navigate(const QUrl& url) {
	QMessageBox::StandardButton response = QMessageBox::No;
	if(mIsInParty) {
		response = QMessageBox::question(this, "Leave Trek?", "You are currently on a Trek.  Navigating away will remove you from the party.  Do you still want to load the page?", QMessageBox::Yes|QMessageBox::No);
		if(response == QMessageBox::Yes) {
			expeditionLeave();
		}
	}
	if(mIsInParty == false || response == QMessageBox::Yes) {
		//QPalette p = addressBar->palette();
		//p.setColor(QPalette::Base, Qt::blue);
		//addressBar->setPalette(p);

		if(webView) {
			webView->stop();
			webView->setFocus();
			webView->load(url);
			if(addressBar) {
				addressBar->setEditText(url.toString());
			}
		}
	} 
} 

void MainWindow::navigate(const QString& address) {
	QString correctedAddress = address;

	if(correctedAddress.startsWith(QString("http://")) == false) {
		correctedAddress.prepend(QString("http://"));
		correctedAddress = correctedAddress.trimmed();
	}

	navigate(QUrl(correctedAddress));
}


void MainWindow::userListWidgetClicked(UserData* data) {
	if(data == NULL || browsingWidget->avatarsVisible() == false) return;
	GraphicsAvatarItem* item = 0;
	if(data == userData) {
		item = userAvatar;
	} else {
		item = contempAvatars.value(data->guid());
	}

		if(!item) return; 

       		QPointF point = item->pos();	
		float percentageX = point.x()/(graphicsScene->width());
		float percentageY = point.y()/(graphicsScene->height());

//		int xOffset = graphicsView->viewport()->width() * (percentageX > .5)?-1:1;
//		int yOffset = graphicsView->viewport()->height() * (percentageY > .5)?-1:1;

		QWebFrame* frame = webView->page()->mainFrame();
		frame->setScrollPosition(QPoint(frame->scrollBarMaximum(Qt::Horizontal)*percentageX, frame->scrollBarMaximum(Qt::Vertical)*percentageY)); 
		item->highlight();	
}

void MainWindow::setupCentralWidget() {
	mNewTabButton.setText("new");
	mNewTabButton.setAutoRaise(true);
	mCloseTabButton.setText("x");
	mCloseTabButton.setAutoRaise(true);
	connect(&mNewTabButton, SIGNAL(clicked()), this, SLOT(newTab()));
	connect(&mCloseTabButton, SIGNAL(clicked()), this, SLOT(closeTab()));
	tabWidget.setCornerWidget(&mNewTabButton,Qt::TopLeftCorner);
	tabWidget.setCornerWidget(&mCloseTabButton, Qt::TopRightCorner);
	tabWidget.setDocumentMode(true);
	tabWidget.setMovable(true);
	tabWidget.setTabsClosable(true);
	connect(&tabWidget, SIGNAL(currentChanged(QWidget*)), this, SLOT(tabChanged()));
	connect(&tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int))); 

	mIntroWidget = new StarfieldWidget();
	mCentralWidget.addWidget(mIntroWidget);
	mCentralWidget.addWidget(&tabWidget);
	setCentralWidget(&mCentralWidget);
}

void MainWindow::setupActions() {
	backAction = new QAction(QIcon(BACK_ICON),tr("Back"), this);
	backAction->setShortcut(QKeySequence::Back);
	forwardAction = new QAction(QIcon(FORWARD_ICON),tr("Forward"), this);
	forwardAction->setShortcut(QKeySequence::Forward);
	stopAction = new QAction(QIcon(STOP_ICON),tr("Stop"), this);
	stopAction->setShortcut(QKeySequence(Qt::Key_F4));
	reloadAction = new QAction(QIcon(RELOAD_ICON),tr("Reload"), this);
	reloadAction->setShortcut(QKeySequence::Refresh);

	mExpeditionAction = new QAction(QIcon(SAFARI_ICON), tr("Expedition Options"), this);
	mPageOptionsAction = new QAction(QIcon(PAGE_ICON), tr("Page Options"), this);
	mToolsAction = new QAction(QIcon(CONFIG_ICON), tr("Configuration Options"), this);

#ifdef ADMIN
	mAdminToolAction = new QAction("idgod 1, wtf?", this);
#endif

	mEnableTutorialAction = new QAction("View the Tutorial", this);


	mpNewTabAction = new QAction(tr("Open New Tab"), this);
	mpNewTabAction->setShortcut(QKeySequence::New);
	mpCloseTabAction = new QAction(tr("Close Active Tab"), this);
	mpCloseTabAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_W));

	mChangeStatusAction = new QAction("Change My Status", this);
	mChangeStatusAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));

	mRequestAvatarDesignAction = new QAction("Change My Avatar", this);
	mRequestAvatarDesignAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
	mRequestProfileDesignAction = new QAction("Change My Profile", this);
	mRequestProfileDesignAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_P));

	mToggleChatLogAction = new QAction("Hide Chat Log", this);
	mToggleChatLogAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
	mToggleChatLogAction->setCheckable(true);
	mToggleChatLogAction->setChecked(true);

	//mReportBugAction = new QAction("Report a Bug", this);

	mToggleAvatarsAction = new QAction("Hide Avatars", this);
	mToggleAvatarsAction->setShortcut(QKeySequence(Qt::Key_F12));
	mToggleAvatarsAction->setCheckable(true);
	mToggleAvatarsAction->setChecked(false);

	mOpacitySlider = new QSlider();
	mOpacitySlider->setOrientation(Qt::Horizontal);
	mOpacitySlider->setValue(100);
	connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(setAvatarOpacity(int)));
	mOpacityAction = new QWidgetAction(this);
	mOpacityAction->setDefaultWidget(mOpacitySlider);

//	mResetScaleAction = new QAction("Reset Avatars Scale", this);
//	mResetScaleAction->setShortcut(QKeySequence(Qt::Key_F11));
//	connect(mResetScaleAction, SIGNAL(triggered()), this, SLOT(resetScale()));

	//mScaleSlider = new QSlider();
	//mScaleSlider->setOrientation(Qt::Horizontal);
	//connect(mScaleSlider,SIGNAL(valueChanged(int)), this, SLOT(setAvatarScale(int))); 
	//resetScale();

	//mScaleAction = new QWidgetAction(this);
	//mScaleAction->setDefaultWidget(mScaleSlider);

	mCutAction = new QAction("Cut", this);
	mCutAction->setShortcut(QKeySequence::Cut);

	mCopyAction = new QAction("Copy", this);
	mCopyAction->setShortcut(QKeySequence::Copy);

	mPasteAction = new QAction("Paste", this);
	mPasteAction->setShortcut(QKeySequence::Paste);

	mToggleFavoriteAction = new QAction("Toggle Favorite", this);
	mToggleFavoriteAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
	mToggleFavoriteAction->setCheckable(true);

	mExpeditionManagerAction = new QAction("Browsing Trek Manager", this);
	mExpeditionManagerAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
	mShowExpeditionRequestsAction = new QAction("Show Browsing Trek Requests", this);

	mExpeditionLeaveAction = new QAction("Leave Current Browsing Trek", this);
	
	connect(mToggleFavoriteAction, SIGNAL(triggered()), this, SLOT(alterFavorite()));

	addAction(backAction);
	addAction(forwardAction);
	addAction(stopAction);
	addAction(reloadAction);

	connect(backAction, SIGNAL(triggered()), this, SLOT(back()));
	connect(forwardAction, SIGNAL(triggered()), this, SLOT(forward()));
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));
	connect(reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
	connect(mpNewTabAction, SIGNAL(triggered()), this, SLOT(newTab()));
	connect(mpCloseTabAction, SIGNAL(triggered()), this, SLOT(closeTab()));
	connect(mRequestAvatarDesignAction, SIGNAL(triggered()), this, SLOT(showAvatarDesignWidget()));
	connect(mChangeStatusAction, SIGNAL(triggered()), this, SLOT(changeStatus()));
	connect(mRequestProfileDesignAction, SIGNAL(triggered()), this, SLOT(showProfileDesignWidget()));
	connect(mToggleChatLogAction, SIGNAL(triggered(bool)), this, SLOT(hideChatLog(bool)));  
	connect(mExpeditionManagerAction, SIGNAL(triggered()), this, SLOT(showExpeditionManager()));
	connect(mShowExpeditionRequestsAction, SIGNAL(triggered()), this, SLOT(showExpeditionRequests()));
	connect(mExpeditionLeaveAction, SIGNAL(triggered()), this, SLOT(expeditionLeave()));

	connect(mCutAction, SIGNAL(triggered()), this, SLOT(cut()));
	connect(mCopyAction, SIGNAL(triggered()), this, SLOT(copy()));
	connect(mPasteAction, SIGNAL(triggered()), this, SLOT(paste()));

	connect(mEnableTutorialAction, SIGNAL(triggered()), this, SLOT(reenableTutorial()));

#ifdef ADMIN
	connect(mAdminToolAction, SIGNAL(triggered()), this, SLOT(showAdminWidget()));
#endif

//	mExpeditionManager = ExpeditionRequestWidget();
}

void MainWindow::setupExpeditionMenu() {
	mExpeditionMenu->clear();
	if(mIsInParty == false && mPartyLeader == NULL) {
		mExpeditionAction->setIcon(QIcon(SAFARI_ICON));
		mExpeditionManagerAction->setText("Start a Browsing Trek");
		mExpeditionMenu->addAction(mExpeditionManagerAction);
	} else if(mIsInParty == false && mPartyLeader) {
		mExpeditionAction->setIcon(QIcon(CROWN_ICON));
		mExpeditionManagerAction->setText("Manage the Browsing Party");
		mExpeditionLeaveAction->setText("Disband the Browsing Party");
		mExpeditionMenu->addAction(mExpeditionManagerAction);
		mExpeditionMenu->addAction(mExpeditionLeaveAction);
	} if(mPartyLeader && mIsInParty) {
		mExpeditionAction->setIcon(QIcon(CAP_ICON));
		mExpeditionLeaveAction->setText("Leave " + mPartyLeader->displayName() +  "'s Trek");
		mExpeditionMenu->addAction(mExpeditionLeaveAction);
	}

	mExpeditionMenu->addSeparator();
	mExpeditionMenu->addAction(mShowExpeditionRequestsAction);
}

void MainWindow::setupMenus() {
	mExpeditionMenu = new QMenu();
	setupExpeditionMenu();
	//mExpeditionMenu->addAction(mExpeditionManagerAction);
	//mExpeditionMenu->addAction(mExpeditionLeaveAction);

	mPageOptionsMenu = new QMenu();
//	mPageOptionsMenu->addAction(backAction);
//	mPageOptionsMenu->addAction(forwardAction);
//	mPageOptionsMenu->addAction(stopAction);
//	mPageOptionsMenu->addAction(reloadAction);
	mPageOptionsMenu->addAction(mCutAction);
	mPageOptionsMenu->addAction(mCopyAction);
	mPageOptionsMenu->addAction(mPasteAction);
	mPageOptionsMenu->addSeparator();
	mPageOptionsMenu->addAction(mToggleFavoriteAction);
	mPageOptionsMenu->addSeparator();
	mPageOptionsMenu->addAction(mToggleAvatarsAction);
	mPageOptionsMenu->addAction(mOpacityAction);
	mPageOptionsMenu->addSeparator();
//	mPageOptionsMenu->addAction(mResetScaleAction);
//	mPageOptionsMenu->addAction(mScaleAction);

	mConfigOptionsMenu = new QMenu();
#ifdef ADMIN
	mConfigOptionsMenu->addAction(mAdminToolAction);
	mConfigOptionsMenu->addSeparator();
#endif 
	//mConfigOptionsMenu->addAction(mInviteBetaAction);
	//mConfigOptionsMenu->addSeparator();
	mConfigOptionsMenu->addAction(mEnableTutorialAction);
	mConfigOptionsMenu->addSeparator();
	mConfigOptionsMenu->addAction(mpNewTabAction);
	mConfigOptionsMenu->addAction(mpCloseTabAction);
	mConfigOptionsMenu->addSeparator();
	mConfigOptionsMenu->addSeparator();
	mConfigOptionsMenu->addAction(mChangeStatusAction);
	mConfigOptionsMenu->addAction(mRequestAvatarDesignAction);
	mConfigOptionsMenu->addAction(mRequestProfileDesignAction);
	mConfigOptionsMenu->addSeparator();
	mConfigOptionsMenu->addAction(mToggleChatLogAction);

/*	mDonationMenu= new QMenu();
	mDonationMenu->setTitle("Donations (Thanks!):");
	mOneTimeDonationAction = mDonationMenu->addAction("One Time Donation"); mRecurringThreeAction = mDonationMenu->addAction("Two Dollar Monthly Donation");
	mRecurringFiveAction = mDonationMenu->addAction("Five Dollar Monthly Donation");
	mRecurringTenAction = mDonationMenu->addAction("Ten Dollar Monthly Donation");
//	mConfigOptionsMenu->addMenu(mDonationMenu); */

	QToolButton* expeditionButton = ((QToolButton*)toolBar->widgetForAction(mExpeditionAction));
	expeditionButton->setMenu(mExpeditionMenu);
	expeditionButton->setPopupMode(QToolButton::InstantPopup);

	QToolButton* pageButton = ((QToolButton*)toolBar->widgetForAction(mPageOptionsAction));
	pageButton->setMenu(mPageOptionsMenu);	
	pageButton->setPopupMode(QToolButton::InstantPopup);
	
	QToolButton* configButton = ((QToolButton*)toolBar->widgetForAction(mToolsAction));
	configButton->setMenu(mConfigOptionsMenu);
	configButton->setPopupMode(QToolButton::InstantPopup);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}

void MainWindow::setupToolBar() {
	toolBar = addToolBar("Tool Bar");
	toolBar->setMovable(false);
	toolBar->setFloatable(false);
	toolBar->setAllowedAreas(Qt::TopToolBarArea);

	addressBar = new AddressComboBox();
	addressBar->setEditable(true);
	addressBar->setInsertPolicy(QComboBox::InsertAtTop);
	addressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	addressBar->setDuplicatesEnabled(false);
	connect(addressBar, SIGNAL(activated(const QString&)), this, SLOT(navigate(const QString&)));

	((AddressComboBox*)addressBar)->addressLineEdit()->setFavoriteIcon(QIcon(FAVORITES_EMPTY_ICON));
	connect(((AddressComboBox*)addressBar)->addressLineEdit(), SIGNAL(favoriteIconClicked()), this, SLOT(alterFavorite())); 

	mProgress = new InfiniteProgressWidget();
	mProgress->setMaximumWidth(40); 
	
	toolBar->addAction(backAction);
	toolBar->addAction(forwardAction);
	toolBar->addAction(stopAction);
	toolBar->addAction(reloadAction);
	toolBar->addSeparator();
	toolBar->addWidget(addressBar);
	addressBar->setMinimumWidth(256);
	mProgressAction = toolBar->addWidget(mProgress);
	toolBar->addSeparator();

	toolBar->addAction(mExpeditionAction);
	toolBar->addAction(mPageOptionsAction);
	toolBar->addAction(mToolsAction);

	toolBar->addSeparator();

	toolBar->setIconSize(QSize(28,28));
}

void MainWindow::setupMenu() {}

void MainWindow::setupDocks() {
	setDockOptions(QMainWindow::AnimatedDocks);
	setDockNestingEnabled(true);


        chatDock = new QDockWidget("Chat", this);
	panelsDock = new PanelsDock("Panels");

	addDockWidget(Qt::LeftDockWidgetArea, panelsDock);

	chatWidget = new ChatWidget(this);
	chatWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	connect(chatWidget->textEdit(), SIGNAL(anchorClicked(const QUrl&)), this, SLOT(navigate(const QUrl&)));
	chatDock->setWidget(chatWidget);
	//chatDock->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	chatDock->setFeatures(NULL);
	chatDock->setTitleBarWidget(new QWidget(this));

	tickerDock = new QDockWidget(this);
	mTickerWidget = new TickerWidget();
	tickerDock->setTitleBarWidget(new QWidget(this));
	tickerDock->setWidget(mTickerWidget);
	mTickerWidget->hide();
	tickerDock->hide();

	addDockWidget(Qt::BottomDockWidgetArea, tickerDock, Qt::Vertical);
	addDockWidget(Qt::BottomDockWidgetArea, chatDock, Qt::Vertical);

	//tickerDock->installEventFilter(this);
	//chatWidget->installEventFilter(this);

	connect(mTickerWidget, SIGNAL(clicked(const QString& )), this, SLOT(tickerClicked(const QString& ))); 
	connect(mTickerWidget, SIGNAL(shown()), this, SLOT(tickerShown()));
	connect(mTickerWidget, SIGNAL(hidden()), this, SLOT(tickerHidden()));
	
}

void MainWindow::setupStatusBar() {
	QStatusBar* statusBar = new QStatusBar();
	setStatusBar(statusBar);
}

void MainWindow::setupPanels() {
        usersPanel = new PanelWidget(QIcon(USERS_ICON), QString("Present Users"));
        usersPanel->setWidget(&usersWidget);
        ((PanelsDock*)panelsDock)->addPanel(usersPanel);
	connect(&usersWidget, SIGNAL(privateChat(UserData*)), this, SLOT(openChat(UserData*)));
	connect(&usersWidget, SIGNAL(profile(UserData*)), this, SLOT(showProfile(UserData*)));
	connect(&usersWidget, SIGNAL(highlight(UserData*)), this, SLOT(userListWidgetClicked(UserData*)));
	connect(&usersWidget, SIGNAL(toggleLiked(UserData*)), this, SLOT(toggleFollowed(UserData*)));
	 connect(&usersWidget, SIGNAL(contextMenu(UserData*)), this, SLOT(showUserContextMenu(UserData*)));

//	connect(&usersWidget, SIGNAL(clicked(UserData*)), this, SLOT(userListWidgetClicked(UserData*))); 
//	usersWidget.allUserWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(&usersWidget, SIGNAL(contextMenuRequested(UserData*)), this, SLOT(showUserContextMenu(UserData*)));
	mRelationsTab = new QTabWidget();
	mRelationsTab->setTabPosition(QTabWidget::West);
	mRelationsTab->addTab(&relationshipWidget, "Relationships");
	mPendingFollowerWidget = new PendingFollowerWidget();
	mRelationsTab->addTab(mPendingFollowerWidget, "Pending Requests");

	relationsPanel = new PanelWidget(QIcon(FRIEND_ICON), QString("Relationship Manager"));
	relationsPanel->setWidget(mRelationsTab);
	relationsPanel->setSearchable(true);
	connect(relationsPanel->searchLine(), SIGNAL(textChanged(const QString&)), relationshipWidget.fansWidget()->proxyModel(), SLOT(setFilterFixedString(const QString&)));
	connect(relationsPanel->searchLine(), SIGNAL(textChanged(const QString&)), relationshipWidget.followedWidget()->proxyModel(), SLOT(setFilterFixedString(const QString&)));
        ((PanelsDock*)panelsDock)->addPanel(relationsPanel);
	//connect(&relationshipWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
	connect(&relationshipWidget, SIGNAL(doubleClicked(UserData*)), this, SLOT(showProfile(UserData*)));
	

	//mDlModel.setDownloadList(&mDownloads);			
	//mProxyDlModel.setSourceModel(&mDlModel);
	//mProxyDlModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
	//mDownloadList.setModel(&mProxyDlModel);
	//mProxyDlModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
	//mDownloadList.setItemDelegate(&mDlDelegate);
	//mDownloadList.horizontalHeader()->setStretchLastSection(true);
	//mDownloadList.setSelectionBehavior(QAbstractItemView::SelectRows);
	downloadPanel = new PanelWidget(QIcon(DOWNLOADS_ICON), QString("Downloads (alpha quality)"));
	downloadPanel->setWidget(&downloadWidget);
	downloadPanel->setSearchable(true);
	connect(downloadPanel->searchLine(), SIGNAL(textChanged(const QString&)), downloadWidget.proxyModel(), SLOT(setFilterFixedString(const QString&)));
       ((PanelsDock*)panelsDock)->addPanel(downloadPanel);
       ((PanelsDock*)panelsDock)->setEnabled(downloadPanel, true);

	favoritesPanel = new PanelWidget(QIcon(FAVORITES_ICON), QString("Favorites"));
	//mFavoritesTree.setModel(&mBkModel);
	//favoritesPanel->setWidget(&mFavoritesTree);
	favoritesPanel->setWidget(&bookmarkWidget);
	favoritesPanel->setSearchable(true);
	bookmarkWidget.view()->setSortingEnabled(true);
	connect(favoritesPanel->searchLine(), SIGNAL(textChanged(const QString&)), bookmarkWidget.proxyModel(), SLOT(setFilterFixedString(const QString&)));

        ((PanelsDock*)panelsDock)->addPanel(favoritesPanel);
       	//((PanelsDock*)panelsDock)->setEnabled(favoritesPanel, false);
	//connect(&bookmarkWidget, SIGNAL(urlDoubleClicked(const QUrl&)), this, SLOT(navigate(const QUrl&)));

	historyPanel = new PanelWidget(QIcon(HISTORY_ICON), QString("History (50 Most Recent Only)"));

	mGoIncognitoCheck = new QCheckBox("Disable History Logging");
	connect(mGoIncognitoCheck, SIGNAL(toggled(bool)), this, SLOT(goIncognito(bool)));
	mClearHistoryButton = new QPushButton("Clear My History");
	connect(mClearHistoryButton, SIGNAL(clicked()), this, SLOT(clearHistory()));
	QWidget* incognitoWidget = new QWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(mGoIncognitoCheck);
	layout->addWidget(&historyWidget);
	layout->addWidget(mClearHistoryButton);
	incognitoWidget->setLayout(layout);

	historyPanel->setWidget(incognitoWidget);
	historyPanel->setSearchable(true);
	//Connect the line edit to the SortFilterProxy
	connect(historyPanel->searchLine(), SIGNAL(textChanged(const QString&)), historyWidget.proxyModel(), SLOT(setFilterFixedString(const QString&)));

        ((PanelsDock*)panelsDock)->addPanel(historyPanel);
      	//((PanelsDock*)panelsDock)->setEnabled(historyPanel, false);
	//connect(&historyWidget, SIGNAL(urlDoubleClicked(const QUrl&)), this, SLOT(navigate(const QUrl&)));

	logPanel = new PanelWidget(QIcon(LOG_ICON), QString("Chats and Treks\n(Double Click to Activate)"));
	logPanel->setWidget(&messageCenterWidget);
	((PanelsDock*)panelsDock)->addPanel(logPanel);
	connect(&messageCenterWidget, SIGNAL(personalChatRequested(UserData*)), this, SLOT(openChat(UserData*)));
	connect(&messageCenterWidget, SIGNAL(expeditionRequested(UserData*)), this, SLOT(expeditionAsk(UserData*)));

}

void MainWindow::loadStarted(BrowsingWidget* view) {
        int index = tabWidget.indexOf(view);
        tabWidget.setTabText(tabWidget.indexOf(view), "Loading...");
	tabWidget.setTabIcon(index, QIcon());
	((AddressComboBox*)addressBar)->addressLineEdit()->clearFavicon();


	if(tabWidget.currentIndex() == tabWidget.indexOf(view)) {
		mProgress->start();
		mProgressAction->setVisible(true);
		browsingWidget->hideAvatars();
		mToggleAvatarsAction->setEnabled(false);
	}
}

void MainWindow::titleChanged(const QString& newTitle, BrowsingWidget* view) {
        int index = tabWidget.indexOf(view);
        tabWidget.setTabToolTip(index, newTitle);

	QString title = newTitle.left(32);
	if(title.size() != view->webView()->title().size()) {
		title.append("...");
	}
	tabWidget.setTabText(index, title);

	if(tabWidget.currentIndex() == index) {
		addressBar->setEditText(webView->url().toString());
		setWindowTitle(webView->title() + " - Trek");
	}


}

void MainWindow::iconChanged(BrowsingWidget* view) {
	//QMessageBox::information(NULL, "Main", "Icon Changed");
	int index = tabWidget.indexOf(view);
	QIcon i = QWebSettings::iconForUrl(view->webView()->url());
	tabWidget.setTabIcon(index,i);// view->webView()->icon());	

	if(tabWidget.currentIndex() == index) {
		((AddressComboBox*)addressBar)->addressLineEdit()->setFavicon(i);		
	}
}

void MainWindow::loadFinished(bool okay, BrowsingWidget* view) {
	loadFinished(okay, view, true);
}
void MainWindow::loadFinished(bool okay, BrowsingWidget* view, bool logHistory) {
	GIMME_SETTINGS
	bool isIncognito = settings.value("isIncognito").toBool();
	SETTINGS_DONE

	if(logHistory && okay && isIncognito == false) {
		emit controller_logHistory(view->webView()->title(), view->webView()->url());
		if(mHistoryModel) {
			mHistoryModel->addSite(view->webView()->title(), view->webView()->url()); 
		}
	}

	int index = tabWidget.indexOf(view);

	if(tabWidget.currentIndex() == index) {
		mProgressAction ->setVisible(false);
		mProgress->stop();
		((AddressComboBox*)addressBar)->setPercentComplete(0);
	}

	if(tabWidget.tabText(index) == "Loading...") {
		titleChanged(view->webView()->title(), view);
	}
	if(okay || view->resolvedUrl() == view->webView()->url()) {
		if(tabWidget.currentIndex() == index) {
			emit controller_serverRequestLocations();
			if(userAvatar) {
				userAvatar->setZValue(1);
				userAvatar->setPos(64+webView->page()->currentFrame()->scrollBarValue(Qt::Horizontal),64+webView->page()->currentFrame()->scrollBarValue(Qt::Vertical));
				if(browsingWidget && browsingWidget->isPageResolved()) {
					browsingWidget->scrolled(0,0);

					if(graphicsScene)
						graphicsScene->addItem(userAvatar);
				}

				usersWidget.addUser(userData);
			}
		}

		iconChanged(view);
		mToggleAvatarsAction->setEnabled(true);
		if(mToggleAvatarsAction->isChecked() == false) {
			browsingWidget->showAvatars();
		}
	}

	//	for(int i=0; i<10; i++) addAvatar(new UserData());
	//QSound::play("C:/Users/Jeremy S. Kross/Pictures/done.wav");
}

void MainWindow::loadProgress(int value, BrowsingWidget* view) {
	int index = tabWidget.indexOf(view);
	if(tabWidget.currentIndex() == index) {
		((AddressComboBox*)addressBar)->setPercentComplete(value);
	}
}

	void MainWindow::back() {
		if(webView)
			webView->back();
	}
	void MainWindow::forward() {
		if(webView)
			webView->forward();
	}
	void MainWindow::reload() {
		if(webView)
			webView->reload();
	}
	void MainWindow::stop() {
		if(webView)
			webView->stop();
	}

void MainWindow::clearContemporaryUsers() {
	//Remove the user avatar
	if(userAvatar && graphicsScene) {
		if(graphicsScene->items().contains(userAvatar)) {
			graphicsScene->removeItem(userAvatar);
		}
		graphicsScene->clear();
	}
	if(graphicsView) {
		graphicsView->resetTransform();
	}
	usersWidget.removeAllUsers();

	if(chatWidget && webView)
		chatWidget->reset(webView->title());

	contempAvatars.clear();
	contempData.clear();

}
int MainWindow::newTab(const QUrl& url, bool moveTo) {
	int newIndex = newTab(moveTo);
	BrowsingWidget* b = (BrowsingWidget*)(tabWidget.widget(newIndex));
	if(b && b->webView()) {
		b->webView()->load(url);
	}
	return newIndex;
}
int MainWindow::newTab(bool moveTo) {
	if(mIsInParty && mPartyLeader) {
		QMessageBox::information(this, "Sorry.", "You can't open a new tab because you're currently on a Browsing Trek.");
		return -1;
	}
	BrowsingWidget* browsingWidget = new BrowsingWidget();
	browsingWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(browsingWidget, SIGNAL(titleChanged(const QString&, BrowsingWidget*)), this, SLOT(titleChanged(const QString&, BrowsingWidget*)));
	connect(browsingWidget, SIGNAL(iconChanged(BrowsingWidget*)), this, SLOT(iconChanged(BrowsingWidget*)));
	connect(browsingWidget, SIGNAL(loadFinished(bool, BrowsingWidget*)), this, SLOT(loadFinished(bool, BrowsingWidget*)));
	connect(browsingWidget, SIGNAL(loadStarted(BrowsingWidget*)), this, SLOT(loadStarted(BrowsingWidget*)));
	connect(browsingWidget, SIGNAL(loadProgress(int, BrowsingWidget*)), this, SLOT(loadProgress(int, BrowsingWidget*)));
	connect(browsingWidget, SIGNAL(mouseClicked(int,int)), this, SLOT(mouseClicked(int,int)));
	connect(browsingWidget, SIGNAL(nodeClicked(int, int, NodeInfo)), this, SLOT(domClicked(int, int, NodeInfo)));
	connect(browsingWidget, SIGNAL(downloadRequested(const QNetworkRequest&)), this, SLOT(browsingWidgetDownloadRequested(const QNetworkRequest&)));

	int newIndex = tabWidget.addTab(browsingWidget, "Untitled");
	if(moveTo) {
		tabWidget.setCurrentIndex(newIndex);

		addressBar->setFocus(Qt::OtherFocusReason);
	}
	return newIndex;
}
void MainWindow::closeTab() {
	closeTab(tabWidget.currentIndex());
}
void MainWindow::closeTab(int id) {
	if(tabWidget.count() > 1 && id < tabWidget.count()) {
		int removedIndex = id; //tabWidget.currentIndex();
		BrowsingWidget* removedWebView = (BrowsingWidget*)tabWidget.widget(id);
		if(removedWebView && graphicsView == removedWebView->graphicsView() && graphicsScene == removedWebView->graphicsScene()) {
			QUrl theVoid;
			emit localeChanged(currentLocale, theVoid);	
			currentLocale = theVoid;
			clearContemporaryUsers();
			graphicsView = NULL;
			graphicsScene = NULL;
			browsingWidget = NULL;
			webView = NULL;
		}
		tabWidget.removeTab(removedIndex);

		/*if(removedWebView && graphicsView == removedWebView->graphicsView()) {	
		  graphicsView = browsingWidget->graphicsView();
		  }
		  if(removedWebView && graphicsScene == removedWebView->graphicsScene()) {
		  graphicsScene = browsingWidget->graphicsScene();
		  }*/

		if(removedWebView){ removedWebView->webView()->stop(); removedWebView->deleteLater(); }
	} else {
	}	
}
void MainWindow::tabChanged() {
//	int tabbedIndex = tabWidget.currentIndex();

	if(browsingWidget) {
		disconnect(browsingWidget, SIGNAL(pageResolved(const QUrl&)), this, SLOT(pageResolved(const QUrl&)));
		disconnect(browsingWidget, SIGNAL(avatarsShown()), this, SLOT(resetCorrectOpacity()));
		//		disconnect(browsingWidget, SIGNAL(avatarsShown()), this, SLOT(resetCorrectScale()));
		//	disconnect(browsingWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(createContextMenu(const QPoint&)));
		disconnect(mToggleAvatarsAction, SIGNAL(triggered(bool)), browsingWidget, SLOT(setAvatarsHidden(bool)));
	}
	BrowsingWidget* current = (BrowsingWidget*)tabWidget.currentWidget();
	browsingWidget = current;
	connect(browsingWidget, SIGNAL(pageResolved(const QUrl&)), this, SLOT(pageResolved(const QUrl&)));
	connect(browsingWidget, SIGNAL(avatarsShown()), this, SLOT(resetCorrectOpacity()));
	//connect(browsingWidget, SIGNAL(avatarsShown()), this, SLOT(resetCorrectScale()));
	connect(mToggleAvatarsAction, SIGNAL(triggered(bool)), browsingWidget, SLOT(setAvatarsHidden(bool)));
	//connect(browsingWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(createContextMenu(const QPoint&)));
	if(webView) {
		disconnect(webView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));

		mPageOptionsMenu->removeAction(webView->pageAction(QWebPage::Cut));
		mPageOptionsMenu->removeAction(webView->pageAction(QWebPage::Copy));
		mPageOptionsMenu->removeAction(webView->pageAction(QWebPage::Paste));
	}

	webView = current->webView();
	connect(webView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
	connect(webView, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)));
	//mPageOptionsMenu->addAction(webView->pageAction(QWebPage::Copy));
	//mPageOptionsMenu->addAction(webView->pageAction(QWebPage::Paste));



	//	graphicsScene = current->graphicsScene();
	//	graphicsView = current->graphicsView();

	((AddressComboBox*)addressBar)->setPercentComplete(0);
	addressBar->setEditText(webView->url().toString());
	mProgress->stop();
	mProgressAction->setVisible(false);
	setWindowTitle("Trek");

	iconChanged(browsingWidget);
	bool isPageResolved = browsingWidget->resolvedUrl().isValid() && browsingWidget->resolvedUrl().isEmpty() == false;
	if(isPageResolved) {
		setWindowTitle(webView->title());
		pageResolved(browsingWidget->resolvedUrl());
	}
	if(browsingWidget->loadProgress() == 100) {
		loadFinished(isPageResolved, browsingWidget,false);
	} else if(browsingWidget->loadProgress() > 0) {
		loadStarted(browsingWidget);
		loadProgress(browsingWidget->loadProgress(), browsingWidget); 
	} 
	if(isPageResolved) {
		titleChanged(webView->title(), browsingWidget);
	}

}

void MainWindow::pageResolved(const QUrl& newLocale) {
	if(currentLocale == newLocale) {
		return;		
	} else {
		std::cout << "Hit a new locale\n";	
	}
	clearContemporaryUsers();
	graphicsScene = browsingWidget->graphicsScene();
	graphicsView = browsingWidget->graphicsView();
	/*if(mIsInParty == true && currentLocale != newLocale) {
	  partyNavigate(currentLocale);	
	  QMessageBox::information(this, "Haha", "You can't escape!");
	  return;
	  }*/
	emit localeChanged(currentLocale, newLocale); 
	currentLocale = newLocale;

	//Clear the new GraphicsScene to be filled
	setHeart();

	//	emit controller_heartSetRequested(webView->url());
}

void MainWindow::urlChanged(const QUrl&) {
}

void MainWindow::domClicked(int x, int y,NodeInfo n) {
	int correctedX = x + webView->page()->currentFrame()->scrollBarValue(Qt::Horizontal);
	int correctedY = y + webView->page()->currentFrame()->scrollBarValue(Qt::Vertical);
	if(userAvatar) {
		userAvatar->moveTo(correctedX, correctedY);
		QSound::play("sounds/move.wav");
	}
	//Inform the Server
	int bodyWidth = webView->page()->currentFrame()->contentsSize().width();
	int bodyHeight = webView->page()->currentFrame()->contentsSize().height();	
	emit positionChanged(n.mId, correctedX, correctedY, n.mX, n.mY, n.mWidth, n.mHeight, bodyWidth, bodyHeight);

	//QMessageBox::information(NULL, "MainWindow", QString("%1").arg(n.mId));
}

//See MainWindow::domClicked()
void MainWindow::mouseClicked(int, int) {}

void MainWindow::browsingWidgetDownloadRequested(const QNetworkRequest& request) {
	QString fileName = QDir::homePath()+"/"+QFileInfo(request.url().path()).fileName();
	QString path = QFileDialog::getSaveFileName(this, "Choose a File Name",fileName);
	if(path.isEmpty()) return;
	QNetworkRequest newRequest = request;
	newRequest.setAttribute(QNetworkRequest::User, path);
	emit downloadRequested(newRequest);
	//emit downloadRequested(path, request);
}


void MainWindow::openMessageCenter() {
	if(panelsDock) {
		((PanelsDock*)panelsDock)->openPanel(logPanel);
	}
}
void MainWindow::showExpeditionRequests() {
	openMessageCenter();
	messageCenterWidget.showExpeditionRequests();
}

void MainWindow::alterFavorite() {
	//QMessageBox::information(this, "Sorry", "Favorites are disabled in this build.");
	//	emit controller_toggleFavorite(webView->title(), webView->url());
	//emit controller_heartSetRequested(webView->url()); 
	if(mBookmarkModel == NULL || browsingWidget->isPageResolved() == false) return;
	if(mBookmarkModel->containsUrl(webView->url())) {
		emit controller_removeFavorite(QString(), webView->url());
		mBookmarkModel->removeSite(QString(), webView->url());
	} else {
		emit controller_addFavorite(webView->title(), webView->url());
		mBookmarkModel->addSite(webView->title(), webView->url());
	}
	setHeart();
}
void MainWindow::setHeart() {
	if(mBookmarkModel && mBookmarkModel->containsUrl(webView->url())) {
		((AddressComboBox*)addressBar)->addressLineEdit()->setFavoriteIcon(QIcon(FAVORITES_ICON));
		mToggleFavoriteAction->setChecked(true);
	} else {
		((AddressComboBox*)addressBar)->addressLineEdit()->setFavoriteIcon(QIcon(FAVORITES_EMPTY_ICON));
		mToggleFavoriteAction->setChecked(false);
	}
}
/*void MainWindow::setHeart(const QUrl& url, bool on) {
  if(url == webView->url()) {
  if(on) { 
  ((AddressComboBox*)addressBar)->addressLineEdit()->setFavoriteIcon(QIcon(FAVORITES_ICON));
  mAddFavoriteAction->setChecked(true);
  } else {
  ((AddressComboBox*)addressBar)->addressLineEdit()->setFavoriteIcon(QIcon(FAVORITES_EMPTY_ICON));
  mAddFavoriteAction->setChecked(false);
  }
  }
  }*/

void MainWindow::showProfile(UserData* data) {
	readyView(data)->showProfile();
}

#ifdef ADMIN
void MainWindow::showAdminWidget() {
	AdminCommandWidget* aw = new AdminCommandWidget();
	aw->setAttribute(Qt::WA_DeleteOnClose);	
	aw->show();
}
#endif

/*
   void MainWindow::showBetaInviteWidget() {
   InviteWidget* iw = new InviteWidget();
   iw->setAttribute(Qt::WA_DeleteOnClose);
   iw->show();
   }
   */

void MainWindow::showExpeditionManager() {
	if(mIsInParty == true) {
		QMessageBox::information(this, "Browsing Party Manager", "You Cannot Manage the Current Browing Party Because You Aren't the Leader.");
		return;
	}

	if(mExpeditionManager) {
		mExpeditionManager->close();
		mExpeditionManager->deleteLater();
	}
	mExpeditionManager = new ExpeditionRequestWidget();
	//mExpeditionManager->setAttribute(Qt::WA_DeleteOnClose);

	QList<UserData*> presentUsers;

	QMap<QUuid, UserData*>::const_iterator i;
	for(i=contempData.constBegin(); i!= contempData.constEnd(); ++i) {
		presentUsers.append(i.value());
	}
	mExpeditionManager->addInvitableList("Present Users", presentUsers);
	if(mFollowedModel) {
		QList<UserData*> followedUsers;
		QList<UserData*> fullList  = *mFollowedModel->relationshipList();
		for(int i=0; i<fullList.count(); i++) {			
			if(fullList[i]->status() == ONLINE) {
				followedUsers.append(fullList[i]);	
			}
		}
		mExpeditionManager->addInvitableList("Followed Users", followedUsers);
	}
	if(mFansModel) {
		QList<UserData*> fanUsers;
		QList<UserData*> fullList  = *mFansModel->relationshipList();
		for(int i=0; i<fullList.count(); i++) {			
			if(fullList[i]->status() == ONLINE) {
				fanUsers.append(fullList[i]);	
			}
		}
		mExpeditionManager->addInvitableList("Fan Users", fanUsers);
	}

	mExpeditionManager->setCurrentParty(mPartyData);
	if(mPartyLeader) {
		mExpeditionManager->setPartyColor(mPartyLeader->partyColor());	
	}
	mExpeditionManager->setInvitableList("all");

	if(mPartyLeader) {

	}
	mExpeditionManager->show();
}

void MainWindow::connectionLost() {
	if(isVisible()) {
		close();
		QMessageBox::information(this, "Error Apparent", "You have been disconnected form the server.");
		QApplication::exit(-1);
	}
}

void MainWindow::cut() {
	if(addressBar->hasFocus()) {
		addressBar->lineEdit()->cut();
	} else if(webView && webView->page() && webView->hasFocus()) {
		webView->page()->triggerAction(QWebPage::Cut);
	} else if(chatWidget && chatWidget->hasFocus()) {

	}
}
void MainWindow::copy() {
	if(addressBar && addressBar->hasFocus()) {
		addressBar->lineEdit()->copy();
	} else if(chatWidget && (chatWidget->lineEdit()->hasFocus() || chatWidget->textEdit()->hasFocus())) {
		if(chatWidget->lineEdit()->hasFocus()) {
			chatWidget->lineEdit()->copy();
		} else if(chatWidget->textEdit()->hasFocus()) {
			chatWidget->textEdit()->copy();
		}
	} else if(webView && webView->page()) {
		webView->page()->triggerAction(QWebPage::Copy);
	}
}
void MainWindow::paste() {
	if(addressBar->hasFocus()) {
		addressBar->lineEdit()->paste();
	} else if(chatWidget && chatWidget->lineEdit()->hasFocus()) {
		chatWidget->lineEdit()->paste();
	} else if(webView && webView->page()) {
		webView->page()->triggerAction(QWebPage::Paste);
	}
}

void MainWindow::controller_userOnline(UserData* u) {
	if(mFollowedModel && mFollowedModel->contains(u)) {
		controller_showSystemTrayMessage(u->displayName(), "Is Now Online!");	
	}
}
void MainWindow::controller_userOffline(UserData* u) {
	if(mFollowedModel && mFollowedModel->contains(u)) {
		controller_showSystemTrayMessage(u->displayName(), "Is Now Offline.");	
	}
}

void MainWindow::clearHistory() {
		if(mHistoryModel) {
			mHistoryModel->removeRows(0, mHistoryModel->rowCount(QModelIndex()));
		}
		emit controller_serverClearHistory();
}
void MainWindow::goIncognito(bool enabled) {
	GIMME_SETTINGS
	if(settings.contains("isIncognito") == false) {

	}
	settings.setValue("isIncognito", enabled);	
	SETTINGS_DONE		
}
void MainWindow::helperHideAllPanelsBut(PanelWidget* panel) {
	((PanelsDock*)panelsDock)->setEnabled(usersPanel,false);	
	((PanelsDock*)panelsDock)->setEnabled(relationsPanel,false);	
	((PanelsDock*)panelsDock)->setEnabled(favoritesPanel,false);
	((PanelsDock*)panelsDock)->setEnabled(historyPanel,false);
	((PanelsDock*)panelsDock)->setEnabled(downloadPanel,false);
	((PanelsDock*)panelsDock)->setEnabled(logPanel,false);
	if(panel) ((PanelsDock*)panelsDock)->setEnabled(panel, true);
}
void MainWindow::helperShowAllPanels() {
	((PanelsDock*)panelsDock)->setEnabled(usersPanel);
	((PanelsDock*)panelsDock)->setEnabled(relationsPanel);	
	((PanelsDock*)panelsDock)->setEnabled(favoritesPanel);
	((PanelsDock*)panelsDock)->setEnabled(historyPanel);
	((PanelsDock*)panelsDock)->setEnabled(downloadPanel);
	((PanelsDock*)panelsDock)->setEnabled(logPanel);
}
void MainWindow::panelOpened(PanelWidget* w) {
	QSettings settings("Kukundu Soft", "Trek");
	settings.beginGroup(userData->displayName());
	if(w == usersPanel) {
		if(settings.value("tutorial/users_done").toBool() == false) {
			helperHideAllPanelsBut(usersPanel);
			helperShowMessages(QStringList() << "This is the Users Panel." << "The users currently on the page are catalogued here." << "By mousing over a user in this pane..." << "..you get a list of common actions." << "Clicking on the user, but off the actions will highlight him/her on the page.");
			settings.setValue("tutorial/users_done", true);
		}
	} else if(w == relationsPanel) {
		if(settings.value("tutorial/relations_done").toBool() == false) {
			helperHideAllPanelsBut(relationsPanel);
			helperShowMessages(QStringList() << "This is the Relationship Management Panel." << "In Trek, users can declare themselves as fans of other users." << "Right click on a user and select 'Send a Fan Request' to become a fan." << "Said requests enter into the 'Pending Requests' tab here..." << "If you are accepted as a fan, that user becomes one of your stars." << "You can view your stars' history, favorites, and relationships."); 
			settings.setValue("tutorial/relations_done", true);
		}
	} else if(w == downloadPanel) {
		if(settings.value("tutorial/download_done").toBool() == false) {
			helperHideAllPanelsBut(downloadPanel);
			helperShowMessages(QStringList() << "This is the Downloads Panel." << "Downloads are considered an experimental feature at present." << "We're working to bring this code up to snuff." << "Please bear with us.");
			settings.setValue("tutorial/download_done", true);
		}
	} else if(w == favoritesPanel) {
		if(settings.value("tutorial/favorites_done").toBool() == false) {
			helperHideAllPanelsBut(favoritesPanel);
			helperShowMessages(QStringList() << "This is the Favorites Panel." << "To add a page to the favorites..." << "..click the heart in the top right hand corner of the address bar." << "Clicking again will remove the page." << "Remember, your fans can see your favorites..." << "..so by adding a site, you're making a statement.");
			settings.setValue("tutorial/favorites_done", true);
		}
	} else if(w == historyPanel) {
		if(settings.value("tutorial/history_done").toBool() == false) {
			helperHideAllPanelsBut(historyPanel);
			helperShowMessages(QStringList() << "This is the History Panel." << "Unsurprisingly, your history is stored here." << "At present, only your 50 most recent pages are stored." << "Your fans can see your history, so don't use Trek for any embarassing or private browsing.");
			settings.setValue("tutorial/history_done", true);
		}
	} else if(w == logPanel) {
		if(settings.value("tutorial/log_done").toBool() == false) {
			helperHideAllPanelsBut(logPanel);
			helperShowMessages(QStringList() << "This is the Chats & Treks Panel." << "It stores any ongoing private chats." << "More interestingly, this is where you accept browsing trek invites." << "Trek's namesake is the trek: our terminology for a group browsing the web collectively." << "To start a Trek, click the purple safari hat in the top right.");
			settings.setValue("tutorial/log_done", true);
		}
	}
	settings.endGroup();
}

void MainWindow::tickerShown() {
	if(chatWidget->textEdit()->isVisible()) {
		chatWidget->setMinimumHeight(chatWidget->minimumSizeHint().height()-tickerDock->height());
	}
	tickerDock->show();
}
void MainWindow::tickerHidden() {
	chatWidget->setMinimumHeight(0);
	tickerDock->hide();
}
void MainWindow::tickerClicked(const QString& mesg) {
	std::cout << mesg.toStdString() << "\n";
	if(panelsDock == NULL) return;
	if(mesg.contains("Would Like You")) {
		messageCenterWidget.showExpeditionRequests();
		((PanelsDock*)panelsDock)->openPanel(logPanel);
	} else if(mesg.contains("PM")) {
		messageCenterWidget.showChats();
		((PanelsDock*)panelsDock)->openPanel(logPanel);
	} else if (mesg.contains("Become Your Fan")) {
		mRelationsTab->setCurrentWidget(mPendingFollowerWidget);
		((PanelsDock*)panelsDock)->openPanel(relationsPanel);
	} else if(mesg.contains("Accepted You")) {
		mRelationsTab->setCurrentWidget(&relationshipWidget);
		((PanelsDock*)panelsDock)->openPanel(relationsPanel);
	} else if(mesg.contains("Download")) {
		((PanelsDock*)panelsDock)->openPanel(downloadPanel);
	}


}
void MainWindow::helperShowMessages(const QStringList& messages) {
	for(int i=0; i<messages.count(); i++) {
		controller_showSystemTrayMessage("", messages[i]);
	}
}
	void MainWindow::controller_showSystemTrayMessage(const QString& title, const QString& message) {
		if(mTickerWidget)
			mTickerWidget->addMessage(title + " " + message);
	}

void MainWindow::focusChanged(QWidget*, QWidget* focused) {
	if(focused == NULL) {
		return;
	}

	mCutAction->setText("Cut");
	mCutAction->setEnabled(false);
	mPasteAction->setText("Paste");
	mPasteAction->setEnabled(false);
	mCopyAction->setText("Copy");
	mCopyAction->setEnabled(false);
	if(focused == chatWidget->lineEdit()) {
		mCutAction->setEnabled(true);
		mCutAction->setText("Cut (from chat line)");	
		mCopyAction->setEnabled(true);
		mCopyAction->setText("Copy (from chat line)");
		mPasteAction->setEnabled(true);
		mPasteAction->setText("Paste (to chat line)");
		return;
	} 
	if(focused == chatWidget->textEdit()) {
		mCopyAction->setEnabled(true);
		mCopyAction->setText("Copy (from chat log)");
		return;
	} 

	if(focused == addressBar) {
		mCutAction->setEnabled(true);
		mCutAction->setText("Cut (from address bar)");	
		mCopyAction->setEnabled(true);
		mCopyAction->setText("Copy (from address bar)");
		mPasteAction->setEnabled(true);
		mPasteAction->setText("Paste (to address bar)");
		return;
	}
	if(webView) {
		//mCutAction->setEnabled(true);
		//mCutAction->setText("Cut (from web page)");
		//	if(webView->pageAction(QWebPage::Copy)->isEnabled()) {
		mCopyAction->setEnabled(true);
		mCopyAction->setText("Copy (from web page)");
		//	} 
		//	if(webView->pageAction(QWebPage::Paste)->isEnabled()) {
		mPasteAction->setEnabled(true);
		mPasteAction->setText("Paste (to web page)");
		//	}
		return;
	}
}

void MainWindow::expeditionLeave() {
	if(mPartyLeader) {
		emit controller_serverExpeditionRequestResponse(mPartyLeader->guid(), false);
		mIsInParty = false;
		mPartyLeader = NULL;
		mPartyData.clear();
		//usersWidget.partyUserWidget()->removeAllUsers();
		setupExpeditionMenu();
		userData->setPartyColor(QColor(), false);
	}
}


void MainWindow::openChat(UserData* data) {
	personalChat(data)->show();
}
PersonalMessageWidget* MainWindow::personalChat(UserData* data) {
	if(data->status() != OFFLINE) {
		PersonalMessageWidget* pmw = NULL; //new PersonalMessageWidget();
		if(mCurrentChats.contains(data->guid())==false) {
			pmw = new PersonalMessageWidget();
			pmw->setLocalData(userData);
			pmw->setRemoteData(data);
			mCurrentChats.insert(data->guid(), pmw);
			messageCenterWidget.logPersonalChat(data);
		} else {
			pmw = mCurrentChats[data->guid()];
		}
		//	if(pmw->isVisible() == false) {
		//		mTrayIcon.showMessage(data->displayName(), "Sent a New Personal Message (Check the Message Center");			
		//	}
		//pmw->show();

		//pmw->raise();
		return pmw;
	}
	return new PersonalMessageWidget();
}

QMenu* MainWindow::createUserContextMenu(UserData* data) {
	QMenu* menu = new QMenu("User");
	QMenu* relationMenu = new QMenu("View " + data->displayName() + "'s :");
	QAction* viewProfile = relationMenu->addAction("Profile");	
	relationMenu->addSeparator();

	QAction* viewFriends = relationMenu->addAction("Friends");

	QAction* viewFollowed = relationMenu->addAction("Stars");
	QAction* viewFans = relationMenu->addAction("Fans");
	relationMenu->addSeparator();
	QAction* viewFavorites = relationMenu->addAction("Favorites");
	QAction* viewHistory = relationMenu->addAction("History");
	if(mFollowedModel && mFollowedModel->contains(data)== false && (userData != data)) {
		viewFollowed->setEnabled(false);
		viewFans->setEnabled(false);	
		viewFavorites->setEnabled(false);
		viewHistory->setEnabled(false);
	}
	menu->addMenu(relationMenu);

	menu->addSeparator();
	QAction* followUser = NULL;
	QAction* unfollowUser = NULL;
	QAction* blockUser = NULL;

	if(data != userData) {
		if(mFollowedModel ) {
			if(mFollowedModel->contains(data) == false)
				followUser = menu->addAction("Send a Fan Request to " + data->displayName());
			else
				unfollowUser = menu->addAction("Stop Being " + data->displayName() + "'s Fan");
		}
		if(mFansModel) {
			if(mFansModel->contains(data)) {
				blockUser = menu->addAction("Deny " + data->displayName() + " Your Stardom");
			}
		}
	}
	menu->addSeparator();

	QAction* chatWithUser = menu->addAction("Private Chat w/ " + data->displayName());
	if(data->status() == OFFLINE) {
		chatWithUser->setEnabled(false);
		chatWithUser->setVisible(false);
	}


	QVariant v;
	v.setValue(data);

	if(followUser) {
		followUser->setData(v);
		connect(followUser, SIGNAL(triggered()), this, SLOT(followTriggered()));
	} else if(unfollowUser) {
		unfollowUser->setData(v);
		connect(unfollowUser, SIGNAL(triggered()), this, SLOT(unfollowTriggered()));
	}
	if(blockUser) {
		blockUser->setData(v);
		connect(blockUser, SIGNAL(triggered()), this, SLOT(blockTriggered()));		
	}
	chatWithUser->setData(v);
	connect(chatWithUser, SIGNAL(triggered()), this, SLOT(chatTriggered()));

	viewFollowed->setData(v);
	connect(viewFollowed, SIGNAL(triggered()), this, SLOT(viewFollowedTriggered()));

	viewFans->setData(v);
	connect(viewFans, SIGNAL(triggered()), this, SLOT(viewFansTriggered()));
	viewFriends->setData(v);
	connect(viewFriends, SIGNAL(triggered()), this, SLOT(viewFriendsTriggered()));
	viewProfile->setData(v);
	connect(viewProfile, SIGNAL(triggered()), this, SLOT(viewProfileTriggered()));
	viewFavorites->setData(v);
	connect(viewFavorites, SIGNAL(triggered()), this, SLOT(viewFavoritesTriggered()));
	viewHistory->setData(v);
	connect(viewHistory, SIGNAL(triggered()), this, SLOT(viewHistoryTriggered()));


	viewFriends->setVisible(false);
	viewFriends->setEnabled(false);
	//Set the UserData* as the userData variant on the Action
	return menu;
}

void MainWindow::showUserContextMenu(UserData* data) {
	if(data == NULL) return;
	QMenu* menu = createUserContextMenu(data);
	menu->exec(QCursor::pos());	
	delete menu;
	menu = NULL;
}

QMenu* MainWindow::createWebContextMenu() {
    return NULL;
}

void MainWindow::showContextMenu(const QPoint& pos) {
	QMenu menu;
	QMenu* userMenu = NULL;
	QMenu* link = NULL;
	QMenu* image = NULL;
	if(sender() == webView) {
		//Check if the point is over and avatar	
		QGraphicsItem* item = graphicsView->itemAt(pos); 
		if(item) {
			UserData* data = ((GraphicsAvatarItem*)item)->userData();
			userMenu = createUserContextMenu(data);
			menu.addActions(userMenu->actions());
			menu.addSeparator();

			//	delete userMenu;
			//	userMenu = NULL;
			//userMenu->setTitle(data->displayName() + ":");
			//	menu.addMenu(userMenu);
			//	(createUserContextMenu(data))->exec(QCursor::pos());
		}
		//Check what web elements were over.
		QWebHitTestResult hit = webView->page()->currentFrame()->hitTestContent(pos);
		if(hit.linkUrl().isEmpty() == false) {
			QString linkText;
			if(hit.linkText().count() > 14) {
				linkText = hit.linkText().left(14);
				linkText += "...";	
			} else {
				linkText =hit.linkText();
			}
			link = new QMenu("(Link) " + linkText);
			QAction* a = link->addAction("Open Link in New Tab");
			QVariant v; 
			v.setValue(hit.linkUrl());
			a->setData(v);
			connect(a, SIGNAL(triggered()), this, SLOT(linkInNewTabTriggered()));
			link->addSeparator();

			link->addAction(webView->pageAction(QWebPage::DownloadLinkToDisk));
			link->addAction(webView->pageAction(QWebPage::CopyLinkToClipboard));
			menu.addMenu(link);
		}
		if(hit.pixmap().isNull() == false) {
			image = new QMenu("(Image) " + hit.alternateText());
			image->addAction(webView->pageAction(QWebPage::DownloadImageToDisk));
			image->addAction(webView->pageAction(QWebPage::CopyImageToClipboard));
			menu.addMenu(image);

		}

	} else if(sender() == &relationshipWidget) {
		/*UserData* data = relationshipWidget.currentSelection();
		  if(data) {
		  (createUserContextMenu(data))->exec(QCursor::pos());			
		  return;
		  } */
	} else { //Assume UserDataWidget

	}

	//	menu.addSeparator();
	//	menu.addAction(backAction);
	//	menu.addAction(forwardAction);
	if(menu.actions().count() > 0)
		menu.exec(QCursor::pos());

	if(userMenu) delete userMenu;
	if(link) delete link;
	if(image) delete image;



	/*	QMenu menu;
		QMenu navMenu;
		QMenu linkMenu;
		QMenu imageMenu;

		menu.addAction(backAction);
		menu.addAction(forwardAction);
		menu.addAction(stopAction);
		menu.addAction(reloadAction);
		menu.addSeparator();
		menu.addAction(mpNewTabAction);
		menu.addAction(mpCloseTabAction);
		menu.addSeparator();
		*/
	//navMenu.setTitle("Navigate");
	//menu.addMenu(&navMenu);


	/*	if(webView) {
	//QAction* openLink = webView->pageAction(QWebPage::OpenLink);
	//openLink->setText("Open");
	QAction* copyLink = webView->pageAction(QWebPage::CopyLinkToClipboard);
	copyLink->setText("Copy Address");
	QAction* saveLink = webView->pageAction(QWebPage::DownloadLinkToDisk);
	saveLink->setText("Save As...");
	linkMenu.setTitle("Link");
	//linkMenu.addAction(openLink);
	linkMenu.addAction(copyLink);
	linkMenu.addAction(saveLink);
	menu.addMenu(&linkMenu);

	QAction* copyImage = webView->pageAction(QWebPage::CopyImageToClipboard);
	copyImage->setText("Copy");
	QAction* saveImage = webView->pageAction(QWebPage::DownloadImageToDisk);
	saveImage->setText("Save As...");
	imageMenu.addAction(copyImage);
	imageMenu.addAction(saveImage);
	imageMenu.setTitle("Image");
	menu.addMenu(&imageMenu);
	}

	menu.addMenu(createUserContextMenu(userData)); */

	//menu.exec(QCursor::pos());
}

void MainWindow::showPageOptions() {
}

void MainWindow::showProfileDesignWidget() {
	QScrollArea* sa = new QScrollArea();
	ProfileWidget* w = new ProfileWidget();
	sa->setWindowTitle("My Profile");
	w->setUserData(userData);
	w->setEditable(true);
	sa->setWidget(w);
	sa->setWidgetResizable(true);
	sa->resize(w->width(), sa->height());
	//sa->setAttribute(Qt::WA_DeleteOnClose);
	sa->show();

	connect(w, SIGNAL(canceled()), sa, SLOT(close()));
}
void MainWindow::changeStatus() {
	QString motd = QInputDialog::getText(this, "Enter New Status", "Please Enter a Status Message <br\> (Your current status appears in your profile.)                        ");
	if(motd.isEmpty() == false)
		emit controller_changePersonalMotd(motd);
}
void MainWindow::showAvatarDesignWidget() {
	QWidget* avatarWidget = new QWidget();
	avatarWidget->setWindowTitle("My Avatar");
	QGridLayout* layout = new QGridLayout();
	//UserData* newData = new UserData();
	//*newData = *userData;
	AvatarDesignAndTestWidget* adw = new AvatarDesignAndTestWidget(userData); //newData);
	QPushButton* applyButton = new QPushButton("Apply");
	QPushButton* closeButton = new QPushButton("Close");
	connect(applyButton, SIGNAL(clicked()), adw->designWidget(), SLOT(commit()));
	connect(applyButton, SIGNAL(clicked()), avatarWidget, SLOT(close()));
	connect(closeButton, SIGNAL(clicked()), avatarWidget, SLOT(close()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(showDesignApplyMesg()));
	layout->addWidget(adw,0,0,1,2);
	layout->addWidget(applyButton, 1,0,1,1);
	layout->addWidget(closeButton, 1,1,1,1);
	avatarWidget->setLayout(layout);
	//avatarWidget->setAttribute(Qt::WA_DeleteOnClose);
	avatarWidget->show();

}

void MainWindow::showDesignApplyMesg() {
	QMessageBox::information(NULL, "Note:", "Changes will be viewable to other users upon entering a new page.");
}


void MainWindow::downloadStarted(DownloadData*) {
	//mDownloads.append(dlData);
	//mDlModel.downloadAdded(dlData);
}

/*bool MainWindow::eventFilter(QObject* obj, QEvent* evt) {
  if(obj == tickerDock) {
  if(evt->type() == QEvent::Hide) {
//chatDock->resize(chatDock->width(), mOldHeight);
}
} else if(obj == chatWidget) {
if(evt->type() == QEvent::Resize) {
std::cout << "w00t\n";
if(mOldChatWidgetHeight != -1) {
std::cout << "Resizing Chat Widget\n";
mOldChatWidgetHeight = -1;
chatWidget->resize(chatWidget->width(), mOldChatWidgetHeight);
return true;
} else if(mOldChatDockHeight != -1) {
std::cout << "Resizing Chat Dock\n";
mOldChatDockHeight = -1;
chatDock->resize(chatDock->width(), 0);
}
//chatDock->resize(chatDock->width(), 0);
//chatDock->setWidget(chatWidget);

}
} else if(obj == chatDock) {
}

return false;
}*/
