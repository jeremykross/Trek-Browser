#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QStackedWidget>
#include <QList>
#include <QSize>

#include <iostream>

#include "SwitchTransitionWidget.h"
#include "SlideTransition.h"

class SwitchWidget:public QStackedWidget {
	Q_OBJECT
	public:
		SwitchWidget(QWidget* parent=NULL);
		~SwitchWidget();
		QSize sizeHint();
		void next(SwitchTransitionWidget* transition = new SlideTransition());
		void previous(SwitchTransitionWidget* transition=new SlideTransition());
		void setCurrentWidget(QWidget* widget);
		void setCurrentWidget(QWidget* widget, SwitchTransitionWidget* transition);
		void setNextWidget(QWidget* widget, SwitchTransitionWidget* transition);
	signals:
		void done();
	public slots:
		void go();
		void goBack();
	protected slots:
		void transitionComplete(QWidget* desiredWidget);
	protected:
		int mCurrIndex;
		SwitchTransitionWidget* mTransition;
		QWidget* mCurrWidget;
		QWidget* mNextWidget;
};

#endif
