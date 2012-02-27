#ifndef USERINFOWIDGET_H
#define USERINFOWIDGET_H

/* CORE */
#include <QWidget>
#include <QObject>
#include <QGLWidget>
#include <QFrame>
#include <QPushButton>
#include <QHBoxLayout>

/* DRAWING */
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QTransform>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QFontMetrics>
#include <QIcon>
#include <QTimer>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QRect>
#include <QRectF>

#include "Enumerations.h"
#include "Controller.h"
#include "UserData.h"
#include "AvatarDefines.h"
#include "RelationshipWidget.h"

class UserInfoWidget: public QWidget {
	Q_OBJECT
	public:
		UserInfoWidget(QWidget* parent=NULL);
		void setUserData(UserData* userData);
	signals:
		void controller_serverRequestStatus(const QUuid& guid);
	protected slots:
		void setMotd(const QString& motd);
		void userDataAltered();
		void userStatusAltered();
		void motdTick();
	protected:
		void initialize();
		void paintEvent(QPaintEvent* evt);
		void resizeEvent(QResizeEvent* evt);
		void mousePressEvent(QMouseEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);

		UserData* mUserData;
		QBrush mIconBrush;
		QString mStatus;
		QString mMotd;

		QTimer mMotdTimer;
		int mMotdWidth;
		int mMotdX;
		
		bool mMousePressed;
};

#endif
