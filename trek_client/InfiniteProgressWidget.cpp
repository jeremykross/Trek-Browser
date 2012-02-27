#include "InfiniteProgressWidget.h"

InfiniteProgressWidget::InfiniteProgressWidget(QWidget* parent): QWidget(parent){	
	mPaintedStep = -1;
	mCurrStep = 1;
	mDefaultColor = QColor("cornflowerblue");
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) { 
			mColors[i][j] = mDefaultColor;
		}
	}

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(step()));
	mTimer.setInterval(75);

	stop();
}

InfiniteProgressWidget::~InfiniteProgressWidget() {
}
void InfiniteProgressWidget::showEvent(QShowEvent*) {
	start();
}
void InfiniteProgressWidget::hideEvent(QHideEvent*) {
	stop();
}

QSize InfiniteProgressWidget::sizeHint() {
	return QSize(100,100);
}

void InfiniteProgressWidget::start() {
	mTimer.start();
}
void InfiniteProgressWidget::stop() {
	QColor stoppedColor = mDefaultColor.lighter(150);
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			if(i==1 && j==1) continue;
			mColors[i][j] = stoppedColor;//palette().color(QPalette::Normal, QPalette::Window); 
		}
	}
	mCurrStep = 0;
	mTimer.stop();
	update();
}

int InfiniteProgressWidget::toStep(int i, int j) {
	/* 123
 * 456
	 * 789
	 *
	 * 123
	 * 804
	 * 765 
	 */
	  if(j==0) {
		switch(i) {
			case 0: return 1;
			case 1: return 2;
			case 2: return 3;
		}
	  } else if(j==1) {
		switch(i) {
			case 0: return 8;
			case 1: return 0;
			case 2: return 4;
		}
	  } else if(j==2) {
		switch(i) {
			case 0: return 7;
			case 1: return 6;
			case 2: return 5;
		}
	  }

          return -1;
}

void InfiniteProgressWidget::paintEvent(QPaintEvent*) {
	float marginDiv2 = MARGIN/2;
	int cubeDimensionX = (width()-marginDiv2)/3;
	int cubeDimensionY = (height()-marginDiv2)/3;
	QPainter painter(this);
	//painter.setRenderHints(QPainter::Antialiasing);
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			if(mCurrStep == toStep(i,j)) mColors[i][j] = mDefaultColor;
			else if(mPaintedStep != mCurrStep)  {
				if((i==1&&j==1) == false)
					mColors[i][j] = mColors[i][j].lighter(107);//mColors[i][j].lighter(107);
			}

			painter.fillRect(i*cubeDimensionX+marginDiv2, j*cubeDimensionY+marginDiv2, cubeDimensionX-marginDiv2, cubeDimensionY-marginDiv2, QBrush(mColors[i][j]));
		}
	}

	mPaintedStep = mCurrStep;
}


void InfiniteProgressWidget::step() {
	mCurrStep++;
	if(mCurrStep == 9) mCurrStep = 1;
	update();
}
