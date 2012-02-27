#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QPaintEvent>
#include <QPainter>
#include <QLinearGradient>
#include <QPixmap>
#include <QColor>
#include <QMouseEvent>
#include <QSize>
#include <QPen>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>

class CustomButton: public QWidget {
	Q_OBJECT
	public:
		CustomButton(QWidget* parent=NULL);
		QSize sizeHint() const;
		void setPixmap(const QPixmap& pixmap);
		void setColor(const QColor& color);
		void setRandomColor();
		QColor color();
		void setText(const QString& text);
		void setPixmapSize(int width, int height);
	signals:
		void clicked();
	protected:
		void mousePressEvent(QMouseEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);
		void paintEvent(QPaintEvent* evt);
		void initialize();
		QPixmap mPixmap;
		QRect mPixmapRect;
		QString mText;
		QColor mColor;
		QSize mSizeHint;
		bool mPressed;
};

#endif
