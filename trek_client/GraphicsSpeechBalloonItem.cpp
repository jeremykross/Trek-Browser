#include "GraphicsSpeechBalloonItem.h"

float GraphicsSpeechBalloonItem::stOpacity = 1;
float GraphicsSpeechBalloonItem::stScale = 1;

QSound GraphicsSpeechBalloonItem::stSound(":/sounds/speak.wav");

GraphicsSpeechBalloonItem::GraphicsSpeechBalloonItem(QGraphicsItem* parent, UserData* userData):QGraphicsItem(parent) {		
	mDirection = RIGHT;
	setUserData(userData);
	initialize();
}

void GraphicsSpeechBalloonItem::initialize() {
	setVisible(false);
	mCurrentSaying = "";
	mDisplayDurationTimer.setInterval(TIME_OUT);
	mDisplayDurationTimer.setSingleShot(true);
	connect(&mDisplayDurationTimer,SIGNAL(timeout()), this, SLOT(animatedHide()));

	mMaxBalloonWidth = MAX_BALLOON_WIDTH_DEFAULT;
	mMaxBalloonHeight = MAX_BALLOON_HEIGHT_DEFAULT;

	//Setup Animations
	mCloseTimer.setDuration(ANIM_HIDE_DURATION);
	mCloseTimer.setUpdateInterval(10);
	mCloseAnimation.setItem(this);
	mCloseAnimation.setTimeLine(&mCloseTimer);
	mCloseAnimation.setTranslationAt(0, PIXMAP_DIMENSION/2, -10);
	mCloseAnimation.setTranslationAt(1, PIXMAP_DIMENSION/2+7, -10-7);
	mCloseAnimation.setScaleAt(1, 1,1);
	connect(&mCloseTimer, SIGNAL(valueChanged(qreal)), this, SLOT(setOpacity(qreal)));
	connect(&mCloseTimer, SIGNAL(finished()), this, SLOT(hide()));

        mDisplayTimer.setDuration(ANIM_SHOW_DURATION);
        mDisplayTimer.setUpdateInterval(10);
	mDisplayAnimation.setItem(this);
	mDisplayAnimation.setTimeLine(&mDisplayTimer);
	mDisplayAnimation.setTranslationAt(0, PIXMAP_DIMENSION/4,-10/2);
	mDisplayAnimation.setTranslationAt(1, PIXMAP_DIMENSION/2, -10);
	mDisplayAnimation.setScaleAt(0,0,0);
	mDisplayAnimation.setScaleAt(.25, 1.35, 1.35);
	mDisplayAnimation.setScaleAt(.65, .8, .8);
	mDisplayAnimation.setScaleAt(1, 1,1); 

	//Setup the brushes

	//Setup the Curly
	QPolygon curlyPoly;
	curlyPoly << QPoint(0,0) << QPoint(CURLY_TO_HOLE, -CURLY_HEIGHT) << QPoint(CURLY_TO_HOLE+CURLY_HOLE, -CURLY_HEIGHT) << QPoint(0,0);
	mCurlyPath.addPolygon(curlyPoly);

	mOpacity = 1.0f;

	scale(0,0);
	setAcceptsHoverEvents(true);
}

void GraphicsSpeechBalloonItem::hoverEnterEvent(QGraphicsSceneHoverEvent* evt) {
	mOpacity = .25f;
	QGraphicsItem::hoverEnterEvent(evt);	
}
void GraphicsSpeechBalloonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* evt) {
	mOpacity = 1.0f;
	QGraphicsItem::hoverLeaveEvent(evt);
}

QRectF GraphicsSpeechBalloonItem::boundingRect() const {
	//This must be made precise!	
	QRectF rect(mBubbleRect.x()-5, mBubbleRect.y()-5,mBubbleRect.width()+10,mBubbleRect.height()+10+CURLY_HEIGHT);
	if(mDirection == LEFT) {
		rect.translate(-(PIXMAP_DIMENSION+rect.width()-15-SHADOW_OFFSET_X), 0);
	}
	return rect;
}

void GraphicsSpeechBalloonItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {

	if(mDirection == LEFT) { 
		painter->scale(-1,1);
		painter->translate(PIXMAP_DIMENSION, 0);
	}

	painter->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);	
	painter->setOpacity(stOpacity*mOpacity);
	
	painter->setPen(Qt::NoPen);
	if(stOpacity*mOpacity > .95) {
	//Draw the drop shadow
	int shadowOffsetX = SHADOW_OFFSET_X;
	int shadowOffsetY = SHADOW_OFFSET_Y;
	if(mDirection == LEFT) { shadowOffsetX*=(-1); }

	painter->translate(shadowOffsetX, shadowOffsetY);
	painter->setBrush(mShadowBrush);
	painter->drawPath(mCurlyPath);
	painter->drawRoundedRect(mBubbleRect,BALLOON_ROUNDNESS,BALLOON_ROUNDNESS);
	painter->translate(-shadowOffsetX, -shadowOffsetY);
	}

	//Draw the Balloon
	painter->setOpacity(stOpacity*mOpacity);
	painter->setBrush(mBalloonBrush);
	painter->drawPath(mCurlyPath);
	painter->drawRoundedRect(mBubbleRect, BALLOON_ROUNDNESS, BALLOON_ROUNDNESS);

	
	//Draw the Text
	QRectF mTextRect = this->mTextRect;

	if(mDirection == LEFT) { 
		painter->scale(-1,1);
		mTextRect = this->mTextRect.translated(-(mTextRect.width()+TEXT_MARGIN/2), 0);	
	//	painter->translate(PIXMAP_DIMENSION, 0);
	}
	painter->setFont(mUserData->font());
	painter->setPen(mUserData->balloonFontColor());
	painter->drawText(mTextRect,Qt::AlignCenter|Qt::TextWordWrap, mCurrentSaying);
}

void GraphicsSpeechBalloonItem::setDirection(Direction d) {
	prepareGeometryChange();
	mDirection = d;	
}

void GraphicsSpeechBalloonItem::setText(const QString& text) {
	mCurrentSaying = text;
	QFontMetrics metric(mUserData->font()); 
	mTextRect = metric.boundingRect(0,0,mMaxBalloonWidth, mMaxBalloonHeight, Qt::AlignCenter|Qt::TextWordWrap, mCurrentSaying);
	//Move the rect into place
	mTextRect.moveLeft(-SIDE_TO_CURLY+TEXT_MARGIN*2);
	mTextRect.moveTop(-(CURLY_HEIGHT+mTextRect.height()));
	mTextRect.translate(-TEXT_MARGIN, -TEXT_MARGIN);
	if(mTextRect.width() < MIN_BALLOON_WIDTH) {
		mTextRect.setWidth(MIN_BALLOON_WIDTH);
	}
	mBubbleRect=mTextRect.adjusted(-TEXT_MARGIN, -TEXT_MARGIN,TEXT_MARGIN, TEXT_MARGIN);

	//Show the Balloon
	mOpacity = 1.0;
        mCloseTimer.stop();
	setVisible(true);
        //stSound.stop();
        //stSound.play();
	animate();

	mDisplayDurationTimer.setInterval(TIME_OUT);
	mDisplayDurationTimer.start();

	parentItem()->setZValue(2);

}

void GraphicsSpeechBalloonItem::setUserData(UserData* data) {
	mUserData = data;
	updateUserData();
	connect(mUserData, SIGNAL(dataAltered()), this, SLOT(updateUserData()));
}

void GraphicsSpeechBalloonItem::updateUserData() {
	mBalloonBrush = QBrush(QColor(mUserData->balloonBGColor().red(), mUserData->balloonBGColor().green(), mUserData->balloonBGColor().blue(), BALLOON_OPACITY));
	mShadowBrush = QBrush(QColor(0,0,0,SHADOW_OPACITY));
}

void GraphicsSpeechBalloonItem::animate() {
	mDisplayTimer.stop();
	mDisplayTimer.setCurrentTime(0);
	mDisplayTimer.setDirection(QTimeLine::Forward);
	mDisplayTimer.start();
}

void GraphicsSpeechBalloonItem::animatedHide() {
	//setVisible(false);
	parentItem()->setZValue(0);
	mCloseTimer.stop();
	mCloseTimer.setCurrentTime(0);
	//mDisplayTimer.setDirection(QTimeLine::Backward);
	mCloseTimer.start();
}

void GraphicsSpeechBalloonItem::setOpacity(qreal value) {
	mOpacity = 1-value;
}

void GraphicsSpeechBalloonItem::hide() {
	setVisible(false);
}
