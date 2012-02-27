#ifndef SWITCHTRANSITIONWIDGET_H
#define SWITCHTRANSITIONWIDGET_H

#include <QWidget>
#include <QGLWidget>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimeLine>

#include <iostream>


class SwitchTransitionWidget: public QWidget {
	Q_OBJECT	
	public:
		SwitchTransitionWidget(QWidget* parent=NULL);	
		~SwitchTransitionWidget();
		void setFromWidget(QWidget* widget);
		void setToWidget(QWidget* widget);
		void setDuration(int duration);
		bool isRunning();
	public slots:
		void go();	
		void goBack();
		void stop();
	signals:
		void done(QWidget* currentWidget);
	protected slots:
		virtual void update(qreal progress);
		virtual void finished();
	protected:
		virtual void paintEvent(QPaintEvent* evt);
	        void initialize();
		void prepareIfReady();

		QTimeLine mTimeLine;
		QWidget* mFrom;
		QWidget* mTo;
		QPixmap* mFromPixmap;
		QPixmap* mToPixmap;
		bool movingForward;
};

#endif
