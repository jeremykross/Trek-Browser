#include "FadeTransition.h"

FadeTransition::FadeTransition(int duration, QWidget* parent):SwitchTransitionWidget(parent) {
	setDuration(duration);
	fromOpacity = 1;
	toOpacity = 0;
}

void FadeTransition::update(qreal progress) {
	fromOpacity = 1-progress;
	toOpacity = progress;

	SwitchTransitionWidget::update(progress);
}

void FadeTransition::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	if(mFromPixmap && mToPixmap == false) return;

	painter.setOpacity(fromOpacity);
	painter.drawPixmap(0,0, *mFromPixmap);
	painter.setOpacity(toOpacity);
	painter.drawPixmap(0,0, *mToPixmap);
}
