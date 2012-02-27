#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define WINDOW_TITLE "P L U T O"
#define PLUTO_DIR    "C:\\Users\\Jeremy S. Kross\\Pictures\\icons\\"//C:/Users/jkross/Pictures/icons/"
#define RESOURCE_DIR ":/icons/"
#define PLUTO_ICON    QString(RESOURCE_DIR) + "pluto_icon.png" //QString(PLUTO_DIR) + "button.png"
#define BACK_ICON     QString(RESOURCE_DIR) + "back.png"
#define FORWARD_ICON  QString(RESOURCE_DIR) + "forward.png"
#define RELOAD_ICON   QString(RESOURCE_DIR) + "refresh.png"
#define STOP_ICON     QString(RESOURCE_DIR) + "stop.png"
#define PAGE_ICON     QString(RESOURCE_DIR) + "pageoptions.png"
#define CONFIG_ICON   QString(RESOURCE_DIR) + "tools.png"

#define USERS_ICON     QString(RESOURCE_DIR) + "users.png"
#define DOWNLOADS_ICON QString(RESOURCE_DIR) + "download.png"
#define FAVORITES_ICON QString(RESOURCE_DIR) + "favorites.png"
#define FAVORITES_EMPTY_ICON QString(RESOURCE_DIR) + "favoritesempty.png"
#define HISTORY_ICON   QString(RESOURCE_DIR) + "history.png"
#define LOG_ICON       QString(RESOURCE_DIR) + "catalog.png"

#define CAP_ICON       QString(RESOURCE_DIR) + "cap.png"
#define SAFARI_ICON    QString(RESOURCE_DIR) + "safari.png"
#define CROWN_ICON     QString(RESOURCE_DIR) + "crown.png"

#define FRIEND_ICON    QString(RESOURCE_DIR) + "friends.png"


/* Core */
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QFont>
#include <QSize>
#include <QMessageBox>
#include <QList>
#include <QSound>
#include <QCursor>
#include <QDir>

/* Web */
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QWebSettings>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QWebHitTestResult>

/* Gui */
#include <QIcon>
#include <QPixmap>
#include <QLabel>
#include <QAction>
#include <QKeySequence>
#include <QWidgetAction>
#include <QMenu>
#include <QToolBar>
#include <QLineEdit>
#include <QDockWidget>
#include <QComboBox>
#include <QTabWidget>  
#include <QAbstractItemView>
#include <QToolButton>
#include <QStatusBar>
#include <QPainter>
#include <QScrollArea>
#include <QTableView>
#include <QTreeView>
#include <QStackedWidget>
#include <QSplitter>
#include <QSlider>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QSortFilterProxyModel>
#include <QCheckBox>
#include <QModelIndex>

/* Graphics View */
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

/* Events */
#include <QPaintEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QKeyEvent>

/* IO */
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>

/* Std */
#include <cstdlib>


#include <QSystemTrayIcon>
#include <QSettings>

/* My Stuff */
#include "AddressComboBox.h"
#include "ChatWidget.h"
#include "BrowsingWidget.h"

#include "UserListPanel.h"
#include "GraphicsAvatarItem.h"
#include "GraphicsSpeechBalloonItem.h"
#include "Controller.h"
#include "PanelsDock.h"
#include "LoginWidget.h"
#include "LoginFeedbackWidget.h"
#include "DownloadData.h"
#include "DownloadWidget.h"
#include "SwitchWidget.h"
#include "SlideTransition.h"
#include "ZoomTransition.h"
#include "InfiniteProgressWidget.h"
#include "StarFieldWidget.h"
#include "SiteListItemModel.h"
#include "BookmarkWidget.h"
#include "HistoryWidget.h"
#include "AvatarDesignWidget.h"
#include "RelationshipModel.h"

#include "RelationshipWidget.h"
#include "RelationshipData.h"

#include "UserInfoWidget.h"
#include "UserDataWidget.h"
#include "ProfileWidget.h"

#include "PersonalMessageWidget.h"

#include "UserDataFactory.h"
#include "Enumerations.h"

#include "SiteListData.h"

#include "ExpeditionRequestWidget.h"

#include "MessageCenterWidget.h"

#include "GraphicsUserListView.h"

#include "AdminCommandWidget.h"

#include "PendingFollowerWidget.h"

#include "TickerWidget.h"

#define NEWLY_ADDED_X 72 
#define NEWLY_ADDED_Y 72
#define INTRO_MESG "Support independent software.  Please consider a donation.  Thanks!" 

#define GIMME_SETTINGS QSettings settings("Kukundu Soft", "Trek"); settings.beginGroup(userData->displayName()); 
#define SETTINGS_DONE settings.endGroup();


class MainWindow : public QMainWindow {
	Q_OBJECT		
	
	public:
		MainWindow();
		~MainWindow();
		void paintEvent(QPaintEvent* evt);
		void prepare();
	signals:
		void connectToServer();
		void localeChanged(const QUrl& oldUrl, const QUrl& newUrl);
		void positionChanged(unsigned int id, int locationX, int locationY, int elementX, int elementY, int elementWidth, int elementHeight, int bodyWidth, int bodyHeight);
		void downloadRequested(const QNetworkRequest& request);

		void appearanceRequested(const QUuid& guid, bool requestIcon, bool requestUsername, bool requestUsernameBGColor, bool requestUsernameFontColor, bool requestBalloonBGColor, bool requestBalloonFontColor);

		void controller_logHistory(const QString& name, const QUrl& url);
		void controller_addFavorite(const QString& name, const QUrl& url);
		void controller_removeFavorite(const QString& name, const QUrl& url);

		void controller_serverRequestLocations();

		void controller_changePersonalMotd(const QString& motd);

		void controller_serverClearHistory();
	//	void controller_toggleFavorite(const QString& name, const QUrl& url);
		//void controller_heartSetRequested(const QUrl& url);

#ifdef ADMIN
		void executeAdminCommand(const QString& username, int command);
#endif

		void followUser(const QUuid& guid);
		void unfollowUser(const QUuid& guid);
		void blockUser(const QUuid& guid);

		void controller_fansModelRequested(const QUuid&);
		void controller_followedModelRequested(const QUuid&);
		void controller_historyModelRequested(const QUuid&);
		void controller_favoritesModelRequested(const QUuid&);

		void controller_serverRequestPendingFollowers();
		void controller_serverRequestPendingFollowing();

		void controller_serverExpeditionRequestResponse(const QUuid& guid, bool);


		void ready();

	protected slots:
		void begin(UserData* myUserData);
		void begun();
		void controller_fansModelOffered(const QUuid& guid, RelationshipListModel* mModel);
		void controller_followedModelOffered(const QUuid& guid, RelationshipListModel* mModel);
		void controller_historyModelReceived(const QUuid& guid, SiteListItemModel* model);
		void controller_favoritesModelReceived(const QUuid& guid, SiteListItemModel* model);

		void partyNavigate(const QUrl& url);
		void navigate(const QUrl& url);
		void navigate(const QString& address);
		void back();
		void forward();
		void stop();
		void reload();
		void tabChanged();
		void urlChanged(const QUrl& url);
		void userListWidgetClicked(UserData* data);
		QMenu* createUserContextMenu(UserData* data);
		QMenu* createWebContextMenu();
		void showUserContextMenu(UserData* data);
		void showContextMenu(const QPoint& pos);
		void showPageOptions();
		void showAvatarDesignWidget();
		void changeStatus();
		void showProfileDesignWidget();
		void showDesignApplyMesg();

		void reenableTutorial();
		void enableTutorial();

		void informDownloadsDisabled();

		void openMessageCenter();
		void showExpeditionRequests();
		void alterFavorite();
		void setHeart();

		void downloadStarted(DownloadData* dlData);

		void pageResolved(const QUrl& newLocale);
		void titleChanged(const QString& newTitle, BrowsingWidget* view);
		void iconChanged(BrowsingWidget* view);
		void loadFinished(bool okay, BrowsingWidget* view);
		void loadFinished(bool okay, BrowsingWidget* view, bool logHistory);
		void loadStarted(BrowsingWidget* view); 
		void loadProgress(int value, BrowsingWidget* view);
			

		void mouseClicked(int x, int y);	
		void domClicked(int x, int y, NodeInfo n);

		void communiqueReceived(const QUuid& guid, const QString& mesg);
		void avatarPositionChanged(const QUuid& guid, unsigned int id, float percentX, float percentY);
		void avatarPositionChangedBackup(const QUuid& guid, float percentX, float percentY);

		void addAvatar(UserData* userData);
		void removeAvatar(const QUuid& guid, const QUrl& newUrl);
		void changeAvatarAppearance(const QUuid& guid, const QPixmap& pixmap, const QString& username, const QColor& usernameBGColor, const QColor& usernameFontColor, const QColor& balloonBGColor, const QColor& balloonFontColor);
		void changeAvatarPartyAppearance(const QUuid& guid, const QColor& partyColor, bool isLEader);

		int newTab(bool moveTo = true);
		int newTab(const QUrl& url, bool moveTo = false);

		void closeTab();
		void closeTab(int id);

		void browsingWidgetDownloadRequested(const QNetworkRequest& request);
		void changeLocalUserData(UserData* data);
		void setAvatarOpacity(int opacity);
		//void resetScale();
		//void setAvatarScale(int scale);

		void hideChatLog(bool value=false);
		void toggleFollowed(UserData* d);
		void followTriggered();
		void unfollowTriggered();
		void blockTriggered();
		void chatTriggered();
		
		void dataWidgetClosed(UserDataWidget* w);
		UserDataWidget* readyView(UserData* data);
		UserDataWidget* readyView(QAction* sender);
		void viewFollowedTriggered();	
		void viewFansTriggered();
		void viewFriendsTriggered();
		void viewProfileTriggered();
		void viewFavoritesTriggered();
		void viewHistoryTriggered();

		void linkInNewTabTriggered();

		void privateChatRequested(const QUuid& guid, const QString& mesg);
		void controller_askingExpeditionJoin(const QUuid& guid); 
		void controller_partyAddUser(UserData* data);
		void controller_partyRemoveUser(const QUuid&);
		void controller_partyEjected();

		void resetCorrectOpacity();
		//void resetCorrectScale();
		
		void showProfile(UserData* d);
		void openChat(UserData* d);
		PersonalMessageWidget* personalChat(UserData* d);

		void expeditionStartAsLeader(const QColor& c);
		void expeditionAsk(UserData* d);

		//void showBetaInviteWidget();

#ifdef ADMIN
		void showAdminWidget();
#endif

		void showExpeditionManager();
		void expeditionLeave();

		void connectionLost();

		void cut();
		void copy();
		void paste();

		void focusChanged(QWidget* old, QWidget* focused);

		void controller_userOnline(UserData* u);
		void controller_userOffline(UserData* u);

		void controller_showSystemTrayMessage(const QString&, const QString&);
		void helperShowMessages(const QStringList& messages);

		void tickerClicked(const QString& mesg);
		void tickerShown();
		void tickerHidden();

		void panelOpened(PanelWidget* w);
		void showTutorial();
		void helperHideAllPanelsBut(PanelWidget* panel);
		void helperShowAllPanels();

		void goIncognito(bool enabled);
		void clearHistory();

	protected:
		void initialize();
		
	//	bool eventFilter(QObject* obj, QEvent* evt);

                void clearContemporaryUsers();
		void setupCentralWidget();
		void setupActions();
		void setupMenus();
		void setupExpeditionMenu();
		void setupToolBar();
		void setupDocks();
		void setupMenu();
		void setupStatusBar();
		void setupPanels();

		void keyReleaseEvent(QKeyEvent* evt);
		void closeEvent(QCloseEvent* evt);

		SwitchWidget mCentralWidget;
		QTabWidget tabWidget;

		/* Web related */	
		QUrl currentLocale;
		BrowsingWidget* browsingWidget;
		QWebView* webView;
		//These represent the graphicsView and Scene of the resolved page.
		QGraphicsView* graphicsView;
		QGraphicsScene* graphicsScene;

		/* Avatars */
		GraphicsAvatarItem* userAvatar;
		UserData* userData;
		
		RelationshipListModel* mFollowedModel;
		RelationshipListModel* mFansModel;
		SiteListItemModel*     mBookmarkModel;
		SiteListItemModel*     mHistoryModel;

		QMap<QUuid, GraphicsAvatarItem*> contempAvatars;
		QMap<QUuid, UserData*> contempData;
		QMap<QUuid, PersonalMessageWidget*> mCurrentChats;
		QMap<QUuid, UserDataWidget*> mCurrentDataWidgets;

		bool mIsInParty;
		UserData* mPartyLeader;
		QList<UserData*> mPartyData;
		
		//QList<DownloadData*> mDownloads;
		
				
		QToolBar* toolBar;
		QAction* backAction;
		QAction* forwardAction;
		QAction* stopAction;
		QAction* reloadAction;
		QAction* mpNewTabAction;
		QAction* mpCloseTabAction;
		QAction* mProgressAction;

		QAction* mExpeditionAction;
		QAction* mShowExpeditionRequestsAction;
		QAction* mPageOptionsAction;
		QAction* mToolsAction;
#ifdef ADMIN
		QAction* mAdminToolAction;
#endif
		QAction* mEnableTutorialAction;
		//QAction* mInviteBetaAction;
		QAction* mChangeStatusAction;
		QAction* mRequestAvatarDesignAction;
		QAction* mRequestProfileDesignAction;
		QAction* mToggleChatLogAction;
		//QAction* mReportBugAction;

		QAction* mExpeditionManagerAction;

		//QMenu* mDonationMenu;
		QAction* mOneTimeDonationAction;
		QAction* mRecurringThreeAction;
		QAction* mRecurringFiveAction;
		QAction* mRecurringTenAction;

		QMenu* mExpeditionMenu;
		QAction* mExpeditionLeaveAction; 

		QMenu* mPageOptionsMenu;
		QAction* mToggleFavoriteAction;
		QAction* mToggleAvatarsAction;
		QAction* mCutAction;
		QAction* mCopyAction;
		QAction* mPasteAction;

		QWidgetAction* mOpacityAction;
		//QAction* mResetScaleAction;
		//QWidgetAction* mScaleAction;

		QSlider *mOpacitySlider;
		//QSlider* mScaleSlider;
		
		QMenu* mConfigOptionsMenu;

		QSplitter* mAddySearchSplitter;
		QComboBox* addressBar;

		QDockWidget* chatDock;
		QDockWidget* usersDock;
		QDockWidget* panelsDock;
		QDockWidget* tickerDock;

		ChatWidget* chatWidget;

		QToolButton mNewTabButton;
		QToolButton mCloseTabButton;

		UserListPanel* userListPanel;
	
		LoginFeedbackWidget* mLoginWidget;

		//QTableView mDownloadList;
		//DownloadModel mDlModel;
		//QSortFilterProxyModel mProxyDlModel;
		//DownloadDelegate mDlDelegate;
		//PanelWidget* downloadPanel;

		GraphicsUserListView usersWidget;	
                PanelWidget* usersPanel;

		CompoundRelationshipWidget relationshipWidget;	
		PanelWidget* relationsPanel;

		PanelWidget* favoritesPanel;
		BookmarkWidget bookmarkWidget;

		PanelWidget* historyPanel;
		HistoryWidget historyWidget;
		QCheckBox* mGoIncognitoCheck;
		QPushButton* mClearHistoryButton;

		PanelWidget* downloadPanel;
		DownloadWidget downloadWidget;

		PanelWidget* logPanel;
		MessageCenterWidget messageCenterWidget;

		InfiniteProgressWidget* mProgress;
		StarfieldWidget* mIntroWidget;

		ExpeditionRequestWidget* mExpeditionManager;

		PendingFollowerWidget* mPendingFollowerWidget;

		TickerWidget* mTickerWidget;

		QTabWidget* mRelationsTab;


		//QSystemTrayIcon mTrayIcon;
};

#endif
