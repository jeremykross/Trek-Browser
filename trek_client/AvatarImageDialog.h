#ifndef AVATARIMAGEDIALOG_H
#define AVATARIMAGEDIALOG_H

#include <QWidget>
#include <QGLWidget>
#include <QDialog>
#include <QObject>
#include <QSize>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QRegion>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTimeLine>
#include <QLinearGradient>
#include <QStackedWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QSizePolicy>
#include <QFileDialog>
#include <QLabel>
#include <QPoint>
#include <QScrollArea>
#include <QScrollBar>
#include <QCheckBox>
#include <QStackedWidget>
#include <QFrame>
#include <QCursor> 
#include <QDir>

#include "SwitchWidget.h"
#include "SlideTransition.h"
#include "FadeTransition.h"

#define RESIZE_THRESHHOLD 10
#define EDGE_WIDTH 2
#define SELECTED_EDGE_WIDTH 7
#define CLIPED_OPACITY 128 

class ImageSelectionWidget:public QWidget {
	Q_OBJECT
	public:
		ImageSelectionWidget(QWidget* parent=NULL);
		QSize sizeHint() const;
		void setPixmap(const QPixmap& pixmap);
	signals:
		void selectionMoved(int centerX, int centerY, int extentX, int extentY);
		void grabbed();
	public slots:
		void grab();
	protected:
		void mousePressEvent(QMouseEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);
		void mouseMoveEvent(QMouseEvent* evt);
		void keyReleaseEvent(QKeyEvent* evt);
		void paintEvent(QPaintEvent* evt);
		void initialize();

		QPixmap mPixmap;
		QPixmap mGrabbedPixmap;
		QRect mSelectionRect;
		bool mSelectionActive;
		
		bool mExpanding;
		bool mExpandingLeft;
		bool mExpandingRight;
		bool mExpandingTop;
		bool mExpandingBottom;

		bool mIsGrabbed;
		int oldX, oldY;

		friend class ImageSelectWidget;
};
class ImageSelectedWidget:public QWidget {
	Q_OBJECT
	public:
		ImageSelectedWidget(QWidget* parent=NULL);		
		void setPixmap(const QPixmap& pixmap, const QRect& rect);
		void setGrabbedPixmap(const QPixmap& pixmap, const QRect& rect);
		QRect pixmapInView();
	public slots:
		void animate();
		void animateReverse();
	protected slots:
		void valueChanged(qreal value);
	protected:
		void paintEvent(QPaintEvent* evt);
		void initialize();
		QPixmap mPixmap;
		QRect mOrigRect;
		QPixmap mGrabbedPixmap;
		QRect mGrabbedRect;
		QRect mClippedRect;
		QTimeLine mTimer;
		qreal rotation;
		float mOpacity;

		friend class ImageSelectWidget;
};

class ImageSelectWidget: public QStackedWidget {
	Q_OBJECT
	public:
		ImageSelectWidget(QWidget* parent=NULL);
		void setPixmap(const QPixmap& pixmap);
		QPixmap selection();
	signals:
		void done();
		void grabbed();
	public slots:
		void grab();
		void tryAgain();
	protected slots:
		void doGrabbed();
		void selectionMoved(int centerX, int centerY, int extentX, int extentY);
		void goBack();
	protected:
		void keyReleaseEvent(QKeyEvent* evt);
		void initialize();
		QScrollArea mScrollArea;
		ImageSelectionWidget mSelectionWidget;
		ImageSelectedWidget mSelectedWidget;
};

class AvatarImageDialog:public QWidget {
	Q_OBJECT;
	public:
       		AvatarImageDialog(QWidget* parent=NULL);
		~AvatarImageDialog();
		QPixmap selection();
	signals:
		void done();
	public slots:
		void selectAFile();
	protected slots:
		void grabbed();
		void tryAgain();
	protected:
		void initialize();
		//QGridLayout mGridLayout;
		QGridLayout mGMainLayout;
		SwitchWidget mSwitch;
		QPushButton* mSelectPushButton;
		QPushButton* mGoBackPushButton;
	
		QWidget mImageSelectPlus;
		ImageSelectWidget mImageSelectWidget;
		QGridLayout mGPlusLayout;

		SwitchWidget mButtonsSwitch;
		QWidget mPlusYesNoButtons;
		QWidget mPlusCropButtonKludge;
		QHBoxLayout mHPlusButtonsLayout;	
		QPushButton* mPlusCropButton;
		QVBoxLayout mKludgeLayout;
		QPushButton* mPlusTryAgainButton;
		QPushButton* mPlusDoneButton;

};

#endif
