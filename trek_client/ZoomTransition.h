#ifndef ZOOMTRANSITION_H
#define ZOOMTRANSITION_H

#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QLinearGradient>
#include <QGLWidget>
#include "SwitchTransitionWidget.h"

class ZoomTransition : public SwitchTransitionWidget {
	public:
		ZoomTransition(int duration=1000, QWidget* parent=NULL);
	protected slots:
		void update(qreal progress);
	protected:
		void paintEvent(QPaintEvent* evt);	
		qreal mScale;
};

#endif
