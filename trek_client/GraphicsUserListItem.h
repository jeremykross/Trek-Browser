#ifndef GRAPHICSUSERLISTITEM_H
#define GRAPHICSUSERLISTITEM_H

#include <QGraphicsItem>
#include <QObject>
#include <QWidget>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QRectF>
#include <QVariant>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QGraphicsProxyWidget>
#include <QPalette>
#include <QApplication>
#include <QTransform>
#include <QTimer>

#include <iostream>

#include "UserSelectionsWidget.h"
#include "UserData.h"

#define DEFAULT_HEIGHT 50
#define DEFAULT_PIXMAP_WIDTH 40
#define UL_HOVER_DURATION 250 
//#define ANIM_MOVE_DURATION 225
#define BOTTOM_SAVED 23

#define ANIM_HIGHLIGHT_DURATION 750 


class GraphicsUserListView;

class GraphicsUserListItem: public QObject, public QGraphicsItem {
	Q_OBJECT
	public:	
		GraphicsUserListItem(UserData* userData=new UserData(), QGraphicsItem* parent=NULL);
		~GraphicsUserListItem();
		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		QVariant itemChange(GraphicsItemChange change, const QVariant& value);

		UserData* userData();
		UserSelectionsWidget* selectionsWidget();
		void updateBounds();
	protected slots:
		void hoverValueChanged(qreal value);
		void moveValueChanged(qreal value);
		void hoverFinished();
		void moveFinished();
		
		void setDesiredPos(QPointF desiredPos, int delay=0, int duration=0);
		void setDesiredPos(float x, float y, int delay=0, int duration=0);
		QPointF desiredPos();
		void moveVertical(int pixels=DEFAULT_HEIGHT, int delay=0, int duration=500);
		void highlight();

		void userDataAltered();
	protected:
		void hoverEnterEvent(QGraphicsSceneHoverEvent* evt);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent* evt);
		void mousePressEvent(QGraphicsSceneMouseEvent* evt);
		UserData* mUserData;

		QPointF mDesiredPos;
		QRectF mBounds;
		QColor mBGColor;
		QColor mFontColor;
		int mPixmapWidth;
		int mWidth;
		int mHeight;
		qreal mPenWidth;

		QTimeLine mHoverTimer;

		QTimeLine mMoveTimer;
		QGraphicsItemAnimation mMoveAnimation;

		QTimeLine mHighlightTimer;
		QGraphicsItemAnimation mHighlightAnimation;

		UserSelectionsWidget* mUserSelections;
		QGraphicsProxyWidget* mUserSelectionsItem; 

		bool mHovering;
		bool mPressed;

	//	QVariant itemChange(GraphicsItemChange change, const QVariant& value);
	//	void updateBounds();

	friend class GraphicsUserListView;
};

#endif
