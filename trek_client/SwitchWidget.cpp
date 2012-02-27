#include "SwitchWidget.h"

SwitchWidget::SwitchWidget(QWidget* parent): QStackedWidget(parent) {
	mTransition = NULL;
	mCurrWidget = mNextWidget = NULL;
	mCurrIndex = 0;
}

SwitchWidget::~SwitchWidget() {
	if(mTransition) {
		//A transition is already running;
		mTransition->stop();
		delete mTransition;
		mTransition = 0;
	}
}

QSize SwitchWidget::sizeHint() {
	return currentWidget()->sizeHint();
}


void SwitchWidget::next(SwitchTransitionWidget* transition) {
	QWidget* toWidget = widget(mCurrIndex+1);
	setNextWidget(toWidget, transition);
	go();
}
void SwitchWidget::previous(SwitchTransitionWidget* transition) {
	QWidget* toWidget = widget(mCurrIndex-1);
	setNextWidget(toWidget, transition);
	go();
}

void SwitchWidget::setCurrentWidget(QWidget* widget) {
	if(currentWidget() != widget) {
		QStackedWidget::setCurrentWidget(widget);
		mCurrIndex = indexOf(widget);
	}
}

void SwitchWidget::setCurrentWidget(QWidget* widget, SwitchTransitionWidget* transition) {
	setNextWidget(widget, transition);
	go();
}
void SwitchWidget::setNextWidget(QWidget* toWidget, SwitchTransitionWidget* transition) {	
	
	if(mTransition) {
		//A transition is already running;
		mTransition->stop();
		delete mTransition;
		mTransition = 0;
	}
	mCurrWidget = widget(mCurrIndex); //QStackedWidget::currentWidget();
	mNextWidget = toWidget;
	mTransition = transition;

//	QStackedWidget::removeWidget(mCurrWidget);
//	QStackedWidget::removeWidget(mNextWidget);

	transition->setToWidget(toWidget);
	transition->setFromWidget(mCurrWidget);

//	QStackedWidget::addWidget(transition);
//	QStackedWidget::setCurrentWidget(transition);

	connect(mTransition, SIGNAL(done(QWidget*)), this, SLOT(transitionComplete(QWidget*)));
	
}
void SwitchWidget::transitionComplete(QWidget* desiredWidget) {
//	QStackedWidget::addWidget(mCurrWidget);
//	QStackedWidget::addWidget(mNextWidget);
	desiredWidget->setFocus(Qt::OtherFocusReason);
	QStackedWidget::setCurrentWidget(desiredWidget);
	mCurrIndex = indexOf(desiredWidget);
	QStackedWidget::removeWidget(mTransition);
	emit done();
}

void SwitchWidget::goBack() {
	if(mTransition) {
		QStackedWidget::addWidget(mTransition);
		QStackedWidget::setCurrentWidget(mTransition);
		mTransition->goBack();		
	}
}

void SwitchWidget::go() {
	if(mTransition) {
		QStackedWidget::addWidget(mTransition);
		QStackedWidget::setCurrentWidget(mTransition);
		mTransition->go();
	}
}
