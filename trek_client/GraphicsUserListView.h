#ifndef GRAPHICSUSERLISTVIEW_H
#define GRAPHICSUSERLISTVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWidget>
#include <QObject>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include <QMap>
#include <QStringList>

#include "GraphicsUserListItem.h"
#include "UserData.h"

class GraphicsUserListView:public QGraphicsView {
	Q_OBJECT
	public:
		GraphicsUserListView();
		void addUser(UserData* userData);
		void removeUser(UserData* userData);
		void removeAllUsers();
	signals:
		void privateChat(UserData* d);
		void profile(UserData* d);
		void toggleLiked(UserData* d); 
		void highlight(UserData* d);
		void contextMenu(UserData* d);
	public slots:
		void addRandomUser();
		void removeRandomUser();
	protected:
		void resizeEvent(QResizeEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);

		void realign();
		QGraphicsScene mScene;

		QStringList mUsernames;
		QMap<QString, GraphicsUserListItem*> mItems;
};

#endif
