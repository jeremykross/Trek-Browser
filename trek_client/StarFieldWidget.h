#ifndef STARFIELD_H
#define STARFIELD_H

/* Core */
#include <QWidget>
#include <QGLWidget>
#include <QList>
#include <QString>
#include <QMessageBox>
#include <QFont>
#include <QTimer>

/* Drawing */
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QLinearGradient>

/* Std */
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>

#define MAX_SCALE 100 
#define SCALE_THRESHHOLD MAX_SCALE/2 

class StarfieldWidget: public QWidget {
	class Star {
		protected:
			Star();
			void set(float x,float y, int destX, int destY);
			void move();

			float mX,mY;
			int mDestX, mDestY;
			float mDirectionX, mDirectionY;
			bool mIsDone;
			float mSpeed;
		friend class StarfieldWidget;
	};
	Q_OBJECT
	public:
		StarfieldWidget(QWidget* parent=NULL);
		~StarfieldWidget();
		void start(int delay = 0);
		void stop();
	signals:
		void done();
	protected slots:
		void updateStars();
	protected:
		void generateStars(int count);	
		void paintEvent(QPaintEvent* evt);
		QList<Star> mStars;
		QTimer mTimer;
		QFont mFont;
		bool mIsRunning;
		float mMaxDistance;

		QPen mPen;
};

#endif
