#include "SwitchTransitionWidget.h"

SwitchTransitionWidget::SwitchTransitionWidget(QWidget*) {
	initialize();
}
SwitchTransitionWidget::~SwitchTransitionWidget() {
	if(mFromPixmap) delete mFromPixmap;
	if(mToPixmap) delete mToPixmap;
	mFromPixmap = mToPixmap = NULL;
}
void SwitchTransitionWidget::initialize() {
	mTimeLine.setDuration(1000);
	mTimeLine.setUpdateInterval(10);	
	mFrom=NULL;
	mTo=NULL;
	mFromPixmap=NULL;
	mToPixmap=NULL;
	movingForward = true;

	connect(&mTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(update(qreal)));
	connect(&mTimeLine, SIGNAL(finished()), this, SLOT(finished()));
}

void SwitchTransitionWidget::finished() {
	if(movingForward && mToPixmap) emit done(mTo);
	else if(mFromPixmap)           emit done(mFrom);
}

void SwitchTransitionWidget::prepareIfReady() {
	if((mTo && mFrom) == false) return;
	//setGeometry(mFrom->geometry());
	//mTo->setGeometry(mFrom->geometry());

	//resize(mFrom->size());
	if(movingForward)
		mTo->resize(mFrom->size());
	else
		mFrom->resize(mTo->size());

	mFromPixmap = new QPixmap(mFrom->size());
	mFrom->render(mFromPixmap);
	mToPixmap = new QPixmap(mTo->size());
	mTo->render(mToPixmap);
}

void SwitchTransitionWidget::setFromWidget(QWidget* widget) {
	mFrom = widget; 
}
void SwitchTransitionWidget::setToWidget(QWidget* widget) {
	mTo = widget;
}

void SwitchTransitionWidget::setDuration(int duration) {
	mTimeLine.setDuration(duration);
}	

bool SwitchTransitionWidget::isRunning() {
	return (mTimeLine.state() == QTimeLine::Running);
}

void SwitchTransitionWidget::update(qreal) {
	QWidget::update();
}
void SwitchTransitionWidget::paintEvent(QPaintEvent* evt) {
	QWidget::paintEvent(evt);
}

void SwitchTransitionWidget::go() {
	movingForward = true;
	mTimeLine.stop();
	mTimeLine.setDirection(QTimeLine::Forward);
	mTimeLine.setCurrentTime(0);

	prepareIfReady();
	mTimeLine.start();
}
void SwitchTransitionWidget::goBack() {
	movingForward = false;
	mTimeLine.stop();
	mTimeLine.setDirection(QTimeLine::Backward);
	mTimeLine.setCurrentTime(mTimeLine.duration());

	prepareIfReady();
	mTimeLine.start();
}

void SwitchTransitionWidget::stop() {
	mTimeLine.stop();
}
