#ifndef FADETRANSITION_H
#define FADETRANSITION_H

#include <QPaintEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <iostream>

#include "SwitchTransitionWidget.h"

class FadeTransition:public SwitchTransitionWidget {
	public:
		FadeTransition(int duration=1000, QWidget* parent=NULL);
	protected slots:
		void update(qreal progress);
	protected: 
		void paintEvent(QPaintEvent* evt);

	float fromOpacity;
	float toOpacity;
};	

#endif
