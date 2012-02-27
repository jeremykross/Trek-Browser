#include "SlideTransition.h"


SlideTransition::SlideTransition(bool inPlace, SlideTransitionDirection direction,int duration, QWidget* parent):SwitchTransitionWidget(parent){
	mDirection=direction;
	mInPlace = inPlace;
	setDuration(duration);
	posX0=posY0=posX1=posY1 = 0;
}

void SlideTransition::update(qreal progress) {
	int myWidth = width();
	int myHeight = height();

	switch(mDirection) {
		case LEFT:
			if(mInPlace==false) posX0 = -(progress * myWidth); 
			posX1 = myWidth - myWidth*progress;
			break;
		case RIGHT:
			if(mInPlace==false) posX0 = (progress * myWidth); 
			posX1 = -myWidth + (myWidth*progress);
			break;
		case UP:
			if(mInPlace == false) posY0 = -(progress*myHeight);
		       	posY1 = myHeight-myHeight*progress;
			break;
		case DOWN:
			if(mInPlace == false) posY0 = myHeight*progress;
			posY1=-myHeight + (progress*myHeight);
			break;
	}
	SwitchTransitionWidget::update(progress);
}

void SlideTransition::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	if(mFromPixmap && mToPixmap == false) return;
//	painter.fillRect(rect(), QBrush(QColor(0,0,0)));
	
	painter.drawPixmap(posX1, posY1, *mToPixmap); //-height()+posY, *mToPixmap);
	painter.drawPixmap(posX0, posY0, *mFromPixmap);
}
