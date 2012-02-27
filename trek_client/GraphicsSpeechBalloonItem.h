#ifndef GRAPHICSSPEECHBALLOONITEM_H
#define GRAPHICSSPEECHBALLOONITEM_H


/* Core */
#include <QGraphicsItem>
#include <QObject>
#include <QWidget>
#include <QString>

/* Appearance */
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QFont>
#include <QFontMetrics>
#include <QColor>
#include <QBrush>

/* Geometry */
#include <QRectF>
#include <QRect>
#include <QPolygonF>
#include <QPolygon>

/* Animation */
#include <QTimer>
#include <QTimeLine>
#include <QGraphicsItemAnimation>

/* Events */
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

/* Misclaneous */
#include <QMessageBox>
#include <QSound>

/* Std */
#include <iostream>

/* Mine */
#include "AvatarDefines.h"
#include "UserData.h"

//Defines for SpeechBalloon
#define SIDE_TO_CURLY 5 
#define CURLY_TO_HOLE  5
#define CURLY_HEIGHT 10
#define CURLY_HOLE 15
#define TEXT_MARGIN 7 
#define MIN_BALLOON_WIDTH 35 
#define MAX_BALLOON_WIDTH_DEFAULT 200 
#define MAX_BALLOON_HEIGHT_DEFAULT 200
#define BALLOON_OPACITY 255
#define BALLOON_ROUNDNESS 5 
#define TIME_OUT 7000
#define ANIM_SHOW_DURATION 400 
#define ANIM_HIDE_DURATION 200



class GraphicsSpeechBalloonItem :public QObject, public QGraphicsItem {	
	Q_OBJECT
	enum Direction {
		LEFT, RIGHT
	};
	public:
		GraphicsSpeechBalloonItem(QGraphicsItem* parent, UserData* userData);
		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		void setText(const QString& text);
		void setDirection(Direction d);
		void setUserData(UserData* data);
		static float stOpacity;
		static float stScale;
	protected slots:
		void hide();
		void setOpacity(qreal value);
		void animate();
		void animatedHide();
		void updateUserData();
	protected:
		void initialize();
		void hoverEnterEvent(QGraphicsSceneHoverEvent* evt);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt);
		unsigned short mMaxBalloonWidth;
		unsigned short mMaxBalloonHeight;

		UserData* mUserData;

		Direction mDirection;

		QPainterPath mCurlyPath;
		QRectF mTextRect;
		QRectF mBubbleRect;

		QBrush mShadowBrush;
		QBrush mBalloonBrush;

		QTimer mDisplayDurationTimer;
		QTimeLine mDisplayTimer;
		QGraphicsItemAnimation mDisplayAnimation;
		QTimeLine mCloseTimer;
		QGraphicsItemAnimation mCloseAnimation;

		QString mCurrentSaying;
		qreal mOpacity;

		static QSound stSound;
		friend class GraphicsAvatarItem;
};
#endif
