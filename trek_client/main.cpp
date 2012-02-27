#include <QApplication>
#include <stdio.h>

#include "MainWindow.h"
#include "ServerConnection.h"
#include "MessageCodec.h"
#include "DownloadManager.h"
#include "RelationshipManager.h"
#include "SiteListManager.h"

#include "ChatWidget.h"
#include "AdminCommandWidget.h"


bool redirectOutput() {
	if(freopen("/dev/null", "w", stdout)) {
		return freopen("/dev/null", "w", stderr);
	} else if(freopen("nul", "w", stdout)) {
		return freopen("nul", "w", stderr);
	}
	return false;
}

int main(int argc, char* argv[]) {
	redirectOutput();

	QApplication::setGraphicsSystem("raster");
	QApplication app(argc, argv);
	app.setWindowIcon(QIcon((QPixmap(PLUTO_ICON).scaled(64,64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation))));

//	InviteWidget w;
//	w.show();

	ServerConnection server;

	MessageCodec messageCodec;
	messageCodec.registerServerConnection(&server);

//	FriendManager friendManager;
//	QObject::connect(Controller::self(), SIGNAL(serverAuthenticationSuccess(UserData*)), &friendManager, SLOT(begin(UserData*)));

        SiteListManager sitesManager;
	RelationshipManager relationshipManager;

	DownloadManager downloadManager;

	//BookmarkManager bookmarkManager;
	//HistoryManager historyManager;
	
//	QSplitter mainSplitter;
//	mainSplitter.setOrientation(Qt::Vertical);

//	TopShelfWidget topShelf;
	MainWindow mainWindow;    

//	mainSplitter.addWidget(&topShelf);
//	mainSplitter.addWidget(&mainWindow);

	mainWindow.prepare();
	QObject::connect(&mainWindow, SIGNAL(ready()), &mainWindow, SLOT(show()));

	return app.exec();
}
