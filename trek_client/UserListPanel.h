#include <QWidget>
#include <QObject>
#include <QPainter>
#include <QBrush>
#include <QLinearGradient>
#include <QPaintEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QFont>
#include <QDebug>

#include <iostream>

#include "UserData.h"
#include "Controller.h"

#define DEFAULT_SCALE     1
#define DEFAULT_SIZE      32 
#define DEFAULT_BETWEEN   5
#define DEFAULT_MAX_SCALE 3
#define DEFAULT_FALLOFF   3

class UserListPanel:public QWidget {
	Q_OBJECT
	public:
		UserListPanel(QWidget* parent=0);
		void paintEvent(QPaintEvent* evt);
		void enterEvent(QEvent* evt);
		void leaveEvent(QEvent* evt);
		void mouseMoveEvent(QMouseEvent* evt);
	public slots:
		void addUser(UserData* userData);
		void removeUser(UserData* userData);
	protected:
		void drawIcon(QPainter* painter, UserData* userData);
		QList<UserData*> mUserData;
		QFont mFont;
		int mMouseX;
		int mMouseY;
		float mMaxScale;
		int mFallOff;
		float mBaseScale;
	        int mBaseSize;
		int mBaseBetween;
};
