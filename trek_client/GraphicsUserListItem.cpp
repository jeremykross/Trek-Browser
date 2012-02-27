#include "GraphicsUserListItem.h"

GraphicsUserListItem::GraphicsUserListItem(UserData* userData, QGraphicsItem* parent):QGraphicsItem(parent) {
	mUserData = userData;	
	connect(mUserData, SIGNAL(dataAltered()), this, SLOT(userDataAltered()));

	mUserSelections = NULL;
	mUserSelectionsItem = NULL;
	
	mUserSelections = new UserSelectionsWidget(mUserData);

	mWidth = 100;
	mHeight = DEFAULT_HEIGHT; 
	mPixmapWidth = DEFAULT_PIXMAP_WIDTH;
	updateBounds();

	mHoverTimer.setDuration(UL_HOVER_DURATION);
	mHoverTimer.setUpdateInterval(10);
	connect(&mHoverTimer, SIGNAL(valueChanged(qreal)), this, SLOT(hoverValueChanged(qreal)));
	connect(&mHoverTimer, SIGNAL(finished()), this, SLOT(hoverFinished()));
	connect(&mMoveTimer, SIGNAL(valueChanged(qreal)), this, SLOT(moveValueChanged(qreal)));
	connect(&mMoveTimer, SIGNAL(finished()), this, SLOT(moveFinished()));

	mHighlightTimer.setDuration(ANIM_HIGHLIGHT_DURATION);
	mHighlightTimer.setUpdateInterval(10);
	mHighlightAnimation.setItem(this);
	mHighlightAnimation.setTimeLine(&mHighlightTimer);
	mHighlightAnimation.setScaleAt(0,1,1);
	mHighlightAnimation.setTranslationAt(0, 0,0);
	mHighlightAnimation.setTranslationAt(.25, 10,0);
	mHighlightAnimation.setScaleAt(.15, 1.2,1.2);
	mHighlightAnimation.setScaleAt(.85, 1.2,1.2);
	mHighlightAnimation.setTranslationAt(.85, 10,0);
	mHighlightAnimation.setTranslationAt(1,0,0);
	mHighlightAnimation.setScaleAt(1, 1,1);
	connect(&mHighlightTimer, SIGNAL(valueChanged(qreal)), this, SLOT(moveValueChanged(qreal)));
	connect(&mHighlightTimer, SIGNAL(finished()), this, SLOT(moveFinished()));

//	mHoverAnimation.setItem(this);
//	mHoverAnimation.setTimeLine(&mHoverTimer);
//	mHoverAnimation.setScaleAt(0,1,1);
//	mHoverAnimation.setScaleAt(1,1,2);

	hoverValueChanged(0);
	hoverFinished();
	setAcceptsHoverEvents(true);

	mHovering = false;
	mPressed = false;
}
GraphicsUserListItem::~GraphicsUserListItem() {
	//Clearing the Scene takes car of this one
	/*if(mUserSelectionsItem) {
		delete mUserSelectionsItem;
		mUserSelectionsItem = NULL;
	}*/
	if(mUserSelections) {
		delete mUserSelections;
		mUserSelections = NULL;
	}

}

UserData* GraphicsUserListItem::userData() {
	return mUserData;
}
UserSelectionsWidget* GraphicsUserListItem::selectionsWidget() {
	return mUserSelections;
}

QRectF GraphicsUserListItem::boundingRect() const{
	return QRectF(mBounds.x()-5, mBounds.y()-5, mBounds.width()+10, mBounds.height()+10);
}

QVariant GraphicsUserListItem::itemChange(GraphicsItemChange change, const QVariant& value) {
	if(change == ItemSceneHasChanged) {
		updateBounds();
	}
	return QGraphicsItem::itemChange(change, value);
}

void GraphicsUserListItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
	painter->setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing|QPainter::TextAntialiasing);
	QPen p(mFontColor);
	p.setWidthF(mPenWidth);
	painter->setPen(p);
	if(mPressed==false)
		painter->setBrush(mBGColor);
	else
		painter->setBrush(mBGColor.darker(125));
	painter->drawRoundedRect(mBounds,5,5);
	painter->drawText(mPixmapWidth+10, DEFAULT_HEIGHT/2+painter->fontMetrics().height()/4, mUserData->displayName());
	painter->drawPixmap(5,5, mPixmapWidth, mPixmapWidth, mUserData->scaledIcon()); 
//	painter->drawRect(mBounds);
//	painter->drawRect(0,0,50,50);
}

void GraphicsUserListItem::updateBounds() {
	mWidth=100;
	if(scene()) mWidth = scene()->width();
	mBounds = QRectF(2,2, mWidth-4, mHeight-4); 
	prepareGeometryChange();
}

void GraphicsUserListItem::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {
	mHoverTimer.stop();
	mHoverTimer.setCurrentTime(0);
	mHoverTimer.setDirection(QTimeLine::Forward);
	mHoverTimer.start(); 
	if(mUserSelectionsItem == NULL) {
		mUserSelectionsItem = new QGraphicsProxyWidget(this);
		mUserSelectionsItem->setWidget(mUserSelections);
		mUserSelectionsItem->setPos(11, DEFAULT_HEIGHT);
		mUserSelectionsItem->setOpacity(0);
	}
	mUserSelectionsItem->setParentItem(this);
//	mUserSelectionsItem->scale(0,0);
	setZValue(5);
	mHovering = true;
	QGraphicsItem::hoverEnterEvent(evt);	
//	mUserSelectionsItem->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
//	mUserSelectionsItem->setOpacity(0);
}
void GraphicsUserListItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {
	mHoverTimer.stop();
	mHoverTimer.setCurrentTime(mHoverTimer.currentTime());
	mHoverTimer.setDirection(QTimeLine::Backward);
	mHoverTimer.start();
	setZValue(3);
	mUserSelectionsItem->setOpacity(1);
	mHovering = false;
	QGraphicsItem::hoverLeaveEvent(evt);

	/*if(scene()) scene()->removeItem(mUserSelectionsItem);
	if(mUserSelectionsItem) { delete mUserSelectionsItem; mUserSelectionsItem = NULL; }
	mUserSelections = NULL;
	setZValue(0); */
	//if(mUserSelections) { delete mUserSelections; mUserSelections = NULL; }
}

void GraphicsUserListItem::userDataAltered() {
	update();	
}

void GraphicsUserListItem::highlight() {
	mHighlightTimer.stop();
	mHighlightTimer.setCurrentTime(0);
	mHighlightTimer.start();	
}

void GraphicsUserListItem::mouseReleaseEvent(QGraphicsSceneMouseEvent*) {
	mPressed = false;
	update();
}
void GraphicsUserListItem::mousePressEvent(QGraphicsSceneMouseEvent*) {
	mPressed = true;
	update();
}

void GraphicsUserListItem::moveFinished() {
	if(mHovering)
		setZValue(3);
	else
		setZValue(1);
}

void GraphicsUserListItem::hoverFinished() {
	if(mHoverTimer.direction() == QTimeLine::Backward) {
		  if(scene()) scene()->removeItem(mUserSelectionsItem);
		  //if(mUserSelectionsItem) { delete mUserSelectionsItem; mUserSelectionsItem = NULL; }
		  //mUserSelections = NULL;
		  mBGColor = QColor(255,255,255,255);
		  setZValue(1);
	}
}

void GraphicsUserListItem::moveValueChanged(qreal value) {
	setZValue(10*mMoveAnimation.verticalScaleAt(value));	
}

void GraphicsUserListItem::hoverValueChanged(qreal value) {
	if(mUserSelections == NULL) return;

	mHeight = DEFAULT_HEIGHT + (5 + mUserSelections->height()/2)*value;	
	QColor bgColor = mUserData->usernameBGColor();	
	int diffR = 255 - bgColor.red();
	int diffG = 255 - bgColor.green();
	int diffB = 255 - bgColor.blue();
	int alpha = 255*(value);
	alpha = 255;
//	if(value <= .01f) alpha = 255;
	if(alpha == 0) alpha = 255;
	mBGColor = QColor::fromRgb(255-diffR*value, 255-diffG*value, 255-diffB*value, alpha);
	QColor fontColor = mUserData->usernameFontColor();
	QColor baseFontColor = Qt::gray;
	diffR = baseFontColor.red() - fontColor.red();
	diffG = baseFontColor.green() - fontColor.green();
	diffB = baseFontColor.blue() - fontColor.blue();
	mFontColor = QColor::fromRgb(baseFontColor.red()-diffR*value, baseFontColor.green()-diffG*value, baseFontColor.blue()-diffB*value);

//	mPixmapWidth = DEFAULT_PIXMAP_WIDTH+DEFAULT_PIXMAP_WIDTH*value;

	if(mUserSelectionsItem && mUserSelections) {
		/*QPalette p =  mUserSelectionsItem->palette();
		QColor c = QApplication::palette().color(QPalette::Button);
		c.setAlphaF(value);
		p.setColor(QPalette::Button,c);  
		mUserSelectionsItem->setPalette(p);
		*/
		//mUserSelectionsItem->setPos(mPixmapWidth, DEFAULT_HEIGHT); 
		mUserSelectionsItem->setOpacity(value);
	//	mUserSelectionsItem->update();
		mUserSelectionsItem->setTransform(QTransform().translate(mUserSelections->width()/2, 0/*mUserSelections->height()/2*/).scale(1, value).translate(-mUserSelections->width()/2, 0/*-mUserSelections->height()/2*/));
	}

	//if(mUserSelectionsItem) mUserSelectionsItem->scale(1, 1);
	//if(mUserSelectionsItem) mUserSelectionsItem->setOpacity(value);

	mPenWidth = 2+2*value;

	updateBounds();

//	int maxPixmap = DEFAULT_HEIGHT*2 - BOTTOM_SAVED;
//	mPixmapWidth = maxPixmap*value;
	//update();
}


void GraphicsUserListItem::setDesiredPos(float x, float y, int delay, int duration) {
	setDesiredPos(QPointF(x,y), delay, duration);
}
void GraphicsUserListItem::setDesiredPos(QPointF desiredPos, int delay, int duration) {
	if(mDesiredPos == desiredPos) return;
	mDesiredPos = desiredPos;
	if(duration) {
		//mMoveTimer.stop();
		mMoveTimer.setDuration(duration);
		mMoveTimer.setUpdateInterval(10);
		mMoveAnimation.setItem(this);
		mMoveAnimation.setTimeLine(&mMoveTimer);
		mMoveAnimation.setPosAt(0, pos());
		mMoveAnimation.setScaleAt(.5, 1.75,1.75);
		mMoveAnimation.setPosAt(.25, pos());
		mMoveAnimation.setPosAt(1, desiredPos);		
		mMoveAnimation.setScaleAt(1,1,1);
		mMoveTimer.setCurrentTime(0);
		if(delay) {
			QTimer::singleShot(delay, &mMoveTimer, SLOT(start()));
		} else {
			mMoveTimer.start();
		}
	} else {
		setPos(mDesiredPos);	
	}
}

QPointF GraphicsUserListItem::desiredPos() {
	return mDesiredPos;
}

void GraphicsUserListItem::moveVertical(int pixels, int delay, int duration) {
	setDesiredPos(QPointF(mDesiredPos.x(), mDesiredPos.y()+pixels), delay, duration);	
}
