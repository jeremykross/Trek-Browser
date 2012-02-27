#include "ZoomTransition.h"

ZoomTransition::ZoomTransition(int duration, QWidget* parent):SwitchTransitionWidget(parent) {
	setDuration(duration);
}

void ZoomTransition::update(qreal progress) {
	mScale = progress;
	SwitchTransitionWidget::update(progress);
}

void ZoomTransition::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing);
	painter.drawPixmap(0,0,*mFromPixmap);
	//painter.fillRect(rect(), QBrush(QColor(0,0,0)));
	painter.translate(mToPixmap->width()/2, mToPixmap->height()/2);
	painter.scale(mScale, mScale);
	painter.drawPixmap(-mToPixmap->width()/2,-mToPixmap->height()/2,*mToPixmap);
	
	painter.setOpacity(.2);
	painter.scale(1,-1);
	painter.translate(0,-mToPixmap->height());
	painter.drawPixmap(-mToPixmap->width()/2,-mToPixmap->height()/2,*mToPixmap);
}
