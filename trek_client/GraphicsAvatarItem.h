#ifndef GRAPHICSAVATARITEM_H
#define GRAPHICSAVATARITEM_H

/* Core Includes */
#include <QGraphicsItem>
#include <QObject>
#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneHoverEvent>

/* Appearance Includes */ 
#include <QStyleOptionGraphicsItem>
#include <QPixmap>
#include <QRect> 
#include <QRectF>
#include <QPoint>
#include <QPointF>
#include <QColor>
#include <QLinearGradient>
#include <QFont>
#include <QFontMetrics>
#include <QPen>
#include <QPainter>
#include <QTransform>
#include <QGraphicsView>
#include <QGraphicsScene>

/* Animation Includes */
#include <QTimeLine>
#include <QGraphicsItemAnimation>
#include <QGraphicsSceneHoverEvent>

/* Misc Includes */
#include <QVariant>
#include <QSound>

/* Std Includes */
#include <math.h>

/* My Includes */
#include "GraphicsSpeechBalloonItem.h"
#include "AvatarDefines.h"
#include "UserData.h"

/* Defines */
#define ROUNDNESS 4 

class GraphicsAvatarItem : public QObject, public QGraphicsItem {
	Q_OBJECT
	public:
		GraphicsAvatarItem(UserData* userData, QGraphicsItem* parent = 0);
		~GraphicsAvatarItem();
		QRectF boundingRect() const;
		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
		QVariant itemChange(GraphicsItemChange change, const QVariant& value);
		void updateBounds();

		UserData* userData();

		static void setGlobalOpacity(float opacity);
		static void setGlobalScale(float scale);
		static float globalScale();
		static float globalOpacity();
		//Hmm.
	public slots: 
		void updateUserData();
		void speak(const QString& text);
		void moveTo(float x, float y);
		void moveTo(const QPointF& position);
		void animatedHide();
		void highlight();
		void setPartyColor(const QColor& c);
		void setLeader(bool isLeader);
		void setLost(bool isLost);
		bool isLost();
	protected slots:
		void resetZValue();
		void removeMe();
		void moveFinished();
	protected:
		void initialize();
		void layoutUsername();
		bool isAnimating();

		void hoverEnterEvent(QGraphicsSceneHoverEvent* evt);
		void hoverLeaveEvent(QGraphicsSceneHoverEvent* evt);
                void hoverMoveEvent(QGraphicsSceneHoverEvent* evt);
		void contextMenuEvent(QGraphicsSceneContextMenuEvent* evt);

		
		UserData* mUserData;
		GraphicsSpeechBalloonItem* mSpeechBalloon;	
		
		/* Appearance Data */
		QBrush mPixmapBrush;
		QBrush mUsernameBrush;
		QPen mUsernamePen;
		QBrush mPartyBrush;
		// need a seperate font variable because the user name is bolded.

		bool mIsLeader;
		bool mIsLost;

		QFont mFont;
		int mFontWidth, mFontHeight;
			
		/* Position Data */
		QPoint mTextLocation;
		QRect mTextUnderlayLocation;
		QRectF bounds;
		static bool stStaticsInitialized;
		static QPointF stIconLocation;
		static QRectF stDropShadowLocation;
		static bool stTransitioning;
		static float stOpacity;
		static float stScale;
		
		//Animation 
		QTimeLine mOpenTimer;
		QGraphicsItemAnimation mOpenAnimation;
		QTimeLine mMoveTimer;
		QGraphicsItemAnimation mMoveAnimation;
		QTimeLine mCloseTimer;
		QGraphicsItemAnimation mCloseAnimation;
		QTimeLine mHighlightTimer;
		QGraphicsItemAnimation mHighlightAnimation;


		float mOpacity;
};
#endif

