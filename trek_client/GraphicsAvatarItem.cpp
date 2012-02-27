#include "GraphicsAvatarItem.h"

//Statics
bool GraphicsAvatarItem::stStaticsInitialized;
QPointF GraphicsAvatarItem::stIconLocation;
QRectF GraphicsAvatarItem::stDropShadowLocation;
float GraphicsAvatarItem::stOpacity = 1;
float GraphicsAvatarItem::stScale = 1;

UserData* GraphicsAvatarItem::userData() {
	return mUserData;
}

void GraphicsAvatarItem::setGlobalOpacity(float opacity) {
	stOpacity = opacity;
	GraphicsSpeechBalloonItem::stOpacity = opacity;
}
void GraphicsAvatarItem::setGlobalScale(float scale) {
	stScale = scale;
	GraphicsSpeechBalloonItem::stScale = scale;
}
float GraphicsAvatarItem::globalScale() {
	return stScale;
}
float GraphicsAvatarItem::globalOpacity() {
	return stOpacity;
}

GraphicsAvatarItem::GraphicsAvatarItem(UserData* userData, QGraphicsItem* parent):QGraphicsItem(parent) {
	mUserData = userData;
	initialize();
	updateBounds();
}
GraphicsAvatarItem::~GraphicsAvatarItem() {}

void GraphicsAvatarItem::initialize() {
	float pixmapDimHalved = PIXMAP_DIMENSION/2;
	/* Initialize the statics:  these are the same for every avatar */
	if(stStaticsInitialized == false) {	
		stIconLocation.setX(-pixmapDimHalved);
		stIconLocation.setY(-pixmapDimHalved);
		stDropShadowLocation.setLeft(-pixmapDimHalved + SHADOW_OFFSET_X);
		stDropShadowLocation.setTop(-pixmapDimHalved + SHADOW_OFFSET_Y);
		stDropShadowLocation.setWidth(PIXMAP_DIMENSION);
		stDropShadowLocation.setHeight(PIXMAP_DIMENSION); 	
		stStaticsInitialized = true;
	}


	mSpeechBalloon = NULL;
	
	//Setup animations
        mOpenTimer.setDuration(ANIM_OPEN_DURATION);
        mOpenTimer.setUpdateInterval(0);
	
        mOpenAnimation.setItem(this);
	mOpenAnimation.setTimeLine(&mOpenTimer);
	mOpenAnimation.setScaleAt(0, 0, 1.75);
	mOpenAnimation.setScaleAt(.25, 1.75, 1.75);
	mOpenAnimation.setScaleAt(.35, 1.75,1.75);
	mOpenAnimation.setScaleAt(.5,1,1);
	mOpenAnimation.setScaleAt(.75, 1.25, 1.25);
	mOpenAnimation.setScaleAt(1,1,1);

	mCloseTimer.setDuration(ANIM_CLOSE_DURATION);
        mCloseTimer.setUpdateInterval(0);
	mCloseAnimation.setItem(this);
	mCloseAnimation.setTimeLine(&mCloseTimer);
	mCloseAnimation.setScaleAt(0, 1,1);
	mCloseAnimation.setScaleAt(1, 0,0);
	connect(&mCloseTimer, SIGNAL(finished()), this, SLOT(removeMe()));

	mHighlightTimer.setDuration(ANIM_HIGHLIGHT_DURATION);
	mHighlightTimer.setUpdateInterval(10);
	mHighlightAnimation.setItem(this);
	mHighlightAnimation.setTimeLine(&mHighlightTimer);
	mHighlightAnimation.setScaleAt(0, 1,1);
	mHighlightAnimation.setScaleAt(.15, 1.5,1.5);
	mHighlightAnimation.setScaleAt(.85, 1.5,1.5);
	mHighlightAnimation.setScaleAt(1, 1,1);
	connect(&mHighlightTimer, SIGNAL(finished()), this, SLOT(resetZValue()));


	connect(&mMoveTimer, SIGNAL(finished()), this, SLOT(moveFinished()));

	updateUserData();
	//scale(1,1);
	mOpacity = 1;
	mIsLeader = false;
	mIsLost = false;
	
	setAcceptsHoverEvents(true);
       // setFlags(QGraphicsItem::ItemIsMovable);


	connect(mUserData, SIGNAL(dataAltered()), this, SLOT(updateUserData()));
}

void GraphicsAvatarItem::layoutUsername() {
	QFontMetrics metric(mFont);
	mFontWidth = metric.width(mUserData->displayName());
	mFontHeight = metric.height();
	
	//Note: Translate UP by SHADOW_OFFSET_Y
	/* mTextLocation */
        mTextLocation.setX(-mFontWidth/2);
	mTextLocation.setY(PIXMAP_DIMENSION/2+NAME_OFFSET+metric.ascent()+SHADOW_OFFSET_Y);
	/* mTextUnderlayLocation */
	mTextUnderlayLocation.setLeft(-mFontWidth/2-NAME_MARGIN);
 	mTextUnderlayLocation.setTop(PIXMAP_DIMENSION/2+NAME_OFFSET-NAME_MARGIN+SHADOW_OFFSET_Y);
	mTextUnderlayLocation.setWidth(mFontWidth+NAME_MARGIN*2);
	mTextUnderlayLocation.setHeight(mFontHeight+NAME_MARGIN*2);
}

void GraphicsAvatarItem::updateBounds() {
	//Some simple math here.
	//Look at it, you'll get it.
	int width = (mFontWidth+(NAME_MARGIN*2)>PIXMAP_DIMENSION+SHADOW_OFFSET_X)?mFontWidth+(NAME_MARGIN*2):PIXMAP_DIMENSION+SHADOW_OFFSET_X;
	//you won't get this, its a hack.
	width += 10; 
	bounds = QRectF(-width/2, -PIXMAP_DIMENSION/2-10, width, PIXMAP_DIMENSION+SHADOW_OFFSET_Y+mFontHeight+NAME_OFFSET+NAME_MARGIN*2+10);
	prepareGeometryChange();
}

QRectF GraphicsAvatarItem::boundingRect() const {
	return QRectF(bounds.x()-5, bounds.y()-5, bounds.width()+10, bounds.height()+10);	
}
void GraphicsAvatarItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
	
	painter->setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing|QPainter::TextAntialiasing);
	painter->setOpacity(stOpacity*mOpacity);

	//Need to do this as opposed to the preferable painter->scale()
	//because the bounding box won't scale and artifacts will result.
	//its a bit of a kludge, come up with something better, WOULD YA?
	if(isAnimating() == false) {
		resetMatrix();
		scale(stScale, stScale);
	}
	//Update: 8.11  And you thought I couldn't do it!
/*	QTransform t = transform();
	t.setMatrix(1, t.m12(), t.m13(), t.m21(), 1, t.m22(), t.m23(), t.m31(), t.m32(), t.m33());
	//Scale base*global
	t.scale(baseXScale*stScale, baseYScale*stScale);
	setTransform(t);*/

	if(mUserData->partyColor().isValid()) {
		if(mUserData->isLeader()) {
			QPen p;
			p.setColor(QColor("gold"));
			p.setWidth(2);
			painter->setPen(p);
		} else {
			painter->setPen(Qt::NoPen);
		}
		painter->setBrush(QBrush(mUserData->partyColor()));
		painter->drawRoundedRect(bounds, 5,5);
	}

	//Draw the Username 
	painter->setBrush(mUsernameBrush);
	painter->setPen(mUsernamePen);
	painter->drawRoundedRect(mTextUnderlayLocation,ROUNDNESS, ROUNDNESS);
	painter->setFont(mFont);//mUserData->font());
	painter->drawText(mTextLocation, mUserData->displayName());
	
	painter->setPen(QPen(Qt::NoPen));
	//Draw the drop shadow
	if(painter->opacity() > .95) {
		painter->setBrush(QColor(0,0,0, SHADOW_OPACITY));
		painter->drawRoundedRect(stDropShadowLocation,ROUNDNESS, ROUNDNESS);
	}
	
	// Draw the pixmap
	painter->setBrush(mPixmapBrush);
	painter->drawRoundedRect(stIconLocation.x(), stIconLocation.y(), PIXMAP_DIMENSION, PIXMAP_DIMENSION, ROUNDNESS, ROUNDNESS);


/*	if(mIsLost) {
		QFont f= painter->font();
		f.setPointSize(24);
		painter->setFont(f);
		painter->setPen(QColor(255,255,255,200));
		painter->setBrush(QColor(0,0,0,128));
		QRect questionRect = painter->fontMetrics().boundingRect("?");
		float questionW = painter->fontMetrics().width("?");
		float questionH = questionRect.height();
		//painter->drawRoundedRect(-(questionW/2)-4, -questionH, questionW+8,questionH, 5,5); 	
		questionRect.translate(-questionRect.width()/4,questionRect.height()/4);
		questionRect.adjust(-4, 0, 2,0); 
		painter->drawRoundedRect(questionRect, 5,5);
		painter->drawText(-questionRect.width()/4, questionRect.height()/4, "?");
	} */
}

QVariant GraphicsAvatarItem::itemChange(GraphicsItemChange change, const QVariant& value) {
	if(change == ItemSceneChange) {
		mMoveTimer.stop();
		mCloseTimer.stop();
		mOpenTimer.stop();
		if(stOpacity > 0) {
			mOpenTimer.setCurrentTime(0);
			mOpenTimer.setDirection(QTimeLine::Forward);
			mOpenTimer.start();	
		}
	} else if(change == ItemChildAddedChange) {
		//Automatically add the speech balloon to this avatar.
		//Note: if more children end up getting added, this code won't work.
		if(mSpeechBalloon == NULL) {
			QGraphicsItem* temp = value.value<QGraphicsItem*>();
			mSpeechBalloon = static_cast<GraphicsSpeechBalloonItem*>(temp);
			//mSpeechBalloon->setUserData(mUserData);
		}
	} else if(change == ItemPositionChange) {
			mOpacity = 1;
	}
	return QGraphicsItem::itemChange(change, value);
}

void GraphicsAvatarItem::updateUserData() {
	mFont = mUserData->font();
	mFont.setBold(true);
	mFont.setPointSize(9);

	mPixmapBrush = QBrush(mUserData->scaledIcon());
	mPixmapBrush.setTransform(QTransform::fromTranslate(-mUserData->scaledIcon().width()/2, -mUserData->scaledIcon().height()/2));
	mUsernameBrush = QBrush(mUserData->usernameBGColor());
	mUsernamePen = QPen(mUserData->usernameFontColor());
	mUsernamePen.setWidth(NAME_OUTLINE_WIDTH);

	mPartyBrush = QBrush(); //QBrush(QColor("cornflowerblue"));

	layoutUsername();

	updateBounds();
}

void GraphicsAvatarItem::speak(const QString& text) {
	if(mSpeechBalloon != NULL && scene() != NULL) {
		if(scenePos().x() > (scene()->width()/2)) {
			mSpeechBalloon->setDirection(GraphicsSpeechBalloonItem::LEFT);
		} else {
			mSpeechBalloon->setDirection(GraphicsSpeechBalloonItem::RIGHT);
		}
		mSpeechBalloon->setVisible(false);
		mSpeechBalloon->setText(text);
		mSpeechBalloon->setVisible(true);
	}
}

void GraphicsAvatarItem::moveTo(float x, float y) {
	moveTo(QPointF(x,y));
}
void GraphicsAvatarItem::moveTo(const QPointF& position) {
	if(position == pos()) return;
	if(mMoveTimer.state() != QTimeLine::Running) {
		QPointF direction = position - pos();
		float length = sqrt((direction.x()*direction.x() + direction.y()*direction.y()));
		direction = direction/length;

                mMoveTimer.setDuration(ANIM_MOVE_DURATION);
                mMoveTimer.setUpdateInterval(10);
		mMoveAnimation.setItem(this);
		mMoveAnimation.setTimeLine(&mMoveTimer);
		mMoveAnimation.setPosAt(0, pos());
		mMoveAnimation.setPosAt(.7, position + direction*10);
		mMoveAnimation.setPosAt(.85, position - direction*5);
		mMoveAnimation.setPosAt(1, position);
		mMoveTimer.setCurrentTime(0);
		mMoveTimer.start();
                //QSound::play(":/sounds/move.wav");
	}

}

void GraphicsAvatarItem::animatedHide() {
	mCloseTimer.stop();
	mCloseTimer.setCurrentTime(0);
	mCloseTimer.start();
}
void GraphicsAvatarItem::highlight() {
	if(mHighlightTimer.state() != QTimeLine::Running) {
		setZValue(10);
		mHighlightTimer.stop();
		mHighlightTimer.setCurrentTime(0);
		mHighlightTimer.start();
		//QSound::play("bip.wav");
	}
}	

void GraphicsAvatarItem::setPartyColor(const QColor& c) {
	QColor color = c;
	color.setAlpha(100);
	mPartyBrush = QBrush(color);
}
void GraphicsAvatarItem::setLeader(bool isLeader) {
	mIsLeader = isLeader;
}
void GraphicsAvatarItem::setLost(bool isLost) {
	mIsLost = isLost;
	update();
}
bool GraphicsAvatarItem::isLost() {
	return mIsLost;
}

void GraphicsAvatarItem::resetZValue() {
	setZValue(0);
}

void GraphicsAvatarItem::moveFinished() {
	if(!scene()) return;
	QGraphicsView* view = scene()->views()[0];
	if(!view) return;
	if(contains(mapFromScene(view->mapToScene(view->mapFromGlobal(QCursor::pos()))))){
			mOpacity = .45f;
	} else {
	}
}
void GraphicsAvatarItem::removeMe() {
	scene()->removeItem(this);
}

bool GraphicsAvatarItem::isAnimating() {
	return mOpenTimer.state() == QTimeLine::Running || mCloseTimer.state() == QTimeLine::Running || mHighlightTimer.state() == QTimeLine::Running; 
}

void GraphicsAvatarItem::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {
	mOpacity = .35f;
	QGraphicsItem::hoverEnterEvent(evt);
}
void GraphicsAvatarItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {
	mOpacity = 1.0f;
	QGraphicsItem::hoverLeaveEvent(evt);
}
void GraphicsAvatarItem::hoverMoveEvent(QGraphicsSceneHoverEvent* evt) {
	QGraphicsItem::hoverMoveEvent(evt);
}

void GraphicsAvatarItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* evt) {
	QGraphicsItem::contextMenuEvent(evt);
}
