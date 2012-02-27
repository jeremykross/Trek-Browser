#include "StarFieldWidget.h"

StarfieldWidget::Star::Star() { 
	mX=mY=mDestX=mDestY=0;
	mSpeed = 2;
}
void StarfieldWidget::Star::set(float x, float y, int destX, int destY) {
	mX = x;
	mY = y;
	mDestX = destX;
	mDestY = destY;
	float directionX = mDestX - mX;
	float directionY = mDestY - mY;
	float length     = sqrt(directionX*directionX + directionY*directionY);
	mDirectionX = directionX/length;
	mDirectionY = directionY/length;
}
void StarfieldWidget::Star::move() {
	mX = mX + mDirectionX*mSpeed;
	mY = mY + mDirectionY*mSpeed;;
}

void StarfieldWidget::generateStars(int count) {
	srand(time(NULL));

	for(int i=0; i<count; i++) {
		Star s;
		//Generate a destination
		int destX, destY;
		if(rand()%2 == 0) {
			destX = (rand()%width());
			if(rand()%2) destY = 0;
			else destY = height();
		} else {
			destY = (rand()%height());
			if(rand()%2) destX = 0;
			else destX = width();
		}
		s.set(width()/2+rand()%300 *(-1*rand()%1), height()/2+rand()%300*(-1*rand()%1), destX,destY);
		s.mSpeed = (8+rand()%15);
		mStars.append(s);	
	}
}

StarfieldWidget::StarfieldWidget(QWidget* parent):QWidget(parent) {
	mTimer.setInterval(5);
	mFont.setPointSize(17);
	mPen.setColor(Qt::white);
	mPen.setWidth(3);
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(updateStars()));
}
StarfieldWidget::~StarfieldWidget() {}
void StarfieldWidget::start(int delay) {
	generateStars(700);
	QTimer::singleShot(delay + 3000, this, SIGNAL(done()));
	QTimer::singleShot(delay, &mTimer, SLOT(start()));
	mIsRunning = true;

	float halfWidth = width()/2;
	float halfHeight = width()/2;
	mMaxDistance = sqrt(halfWidth*halfWidth + halfHeight*halfHeight);
}
void StarfieldWidget::stop() {
	mTimer.stop();
	mIsRunning = false;
}
void StarfieldWidget::updateStars() {
	for(int i=0; i<mStars.count(); i++) {
		mStars[i].move();
	}
	update();
}

void StarfieldWidget::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
	painter.fillRect(rect(), QBrush(QColor(0,0,0)));
	
	for(int i=0; i<mStars.count(); i++) {
		float distance = sqrt((mStars[i].mX-mStars[i].mDestX)*(mStars[i].mX-mStars[i].mDestX) + (mStars[i].mY-mStars[i].mDestY)*(mStars[i].mY-mStars[i].mDestY)); 
		float percentComplete = (1-distance/mMaxDistance);
		float scale;
		scale=(MAX_SCALE) * percentComplete;
		

		float lineX0, lineY0, lineX1, lineY1;
		lineX0 = mStars[i].mX+mStars[i].mDirectionX*scale;//*(scale/2);
		lineY0 = mStars[i].mY+mStars[i].mDirectionY*scale;//*(scale/2);
		lineX1 = mStars[i].mX-mStars[i].mDirectionX;
		lineY1 = mStars[i].mY-mStars[i].mDirectionY;

		painter.setPen(mPen);
		painter.drawLine(lineX0, lineY0, lineX1, lineY1);
	}
	//painter.setBrush(QBrush(QColor(255,255,255)));
	painter.setPen(QColor(255,255,255));
	painter.setFont(mFont);
	painter.setOpacity(.85);
	painter.drawText(50,50,"Welcome to Trek!");
}
