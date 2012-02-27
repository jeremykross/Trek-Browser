#ifndef SLIDETRANSITION_H
#define SLIDETRANSITION_H

#include <QPainter>
#include <QBrush>
#include <QColor>

#include "SwitchTransitionWidget.h"

enum SlideTransitionDirection {
	LEFT, RIGHT, UP, DOWN
};

class SlideTransition: public SwitchTransitionWidget {
	public:
		SlideTransition(bool inPlace=false, SlideTransitionDirection direction = LEFT,int duration=1000, QWidget* parent=NULL);
	protected slots:
		void update(qreal progress);
	protected:
		void paintEvent(QPaintEvent* evt);

		SlideTransitionDirection mDirection;
		bool mInPlace;
		int posX0;
		int posY0;
		int posX1;
		int posY1;

};

#endif
