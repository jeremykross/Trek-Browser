#ifndef INFINITEPROGRESSWIDGET_H
#define INFINITEPROGRESSWIDGET_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QPainter>
#include <QPaintEvent>
#include <QColor>
#include <QBrush>
#include <QTimer>
#include <QSize>
#include <QSizePolicy>
#include <QSound>
#include <QPalette>
#include <QShowEvent>
#include <QHideEvent>

#define MARGIN (width()/25>5?width()/25:5) //5 //width()/25 //25   

class InfiniteProgressWidget: public QWidget {
	Q_OBJECT
	public:
		InfiniteProgressWidget(QWidget* parent=NULL);
		~InfiniteProgressWidget();
		QSize sizeHint();
		void paintEvent(QPaintEvent* evt);
	public slots:
		void start();
		void stop();
	protected slots:
		void step();
	protected:
		void showEvent(QShowEvent* evt);
		void hideEvent(QHideEvent* evt);
		int toStep(int i, int j);
		int mCurrStep;
		int mPaintedStep;
		QColor mDefaultColor;
		QColor mColors[3][3];
		QTimer mTimer;

};

#endif
