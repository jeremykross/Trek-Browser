#include "AvatarImageDialog.h"

ImageSelectionWidget::ImageSelectionWidget(QWidget* parent):QWidget(parent) {
	initialize();
}

void ImageSelectionWidget::initialize() {
	setMouseTracking(true);
	mSelectionRect = QRect(0,0,58,58);	
	mSelectionRect.moveTopLeft(QPoint(50,50));
	mSelectionActive = false;
	oldX = oldY = -1;
}

void ImageSelectionWidget::setPixmap(const QPixmap& pixmap) {
	mPixmap = pixmap;
	setFixedSize(pixmap.size());
	mSelectionRect = QRect(0,0,58,58);
	mSelectionRect.moveTopLeft(QPoint(50,50));
	if(pixmap.rect().contains(mSelectionRect) == false) {
		mSelectionRect = pixmap.rect();
	}
	//mSelectionRect = pixmap.rect();
}

QSize ImageSelectionWidget::sizeHint() const {
	return QSize();
}

void ImageSelectionWidget::paintEvent(QPaintEvent* evt) {
	QPainter painter(this);
	painter.setOpacity(1);
	painter.setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing);
	painter.setClipRegion(evt->region());

	painter.drawPixmap(0,0,mPixmap);
	painter.setBrush(QColor(0,0,0 ,CLIPED_OPACITY));
	QRegion r(rect());
	r = r.subtracted(mSelectionRect);
	painter.setClipRegion(r);// Qt::UniteClip);
	painter.drawRect(rect());
	QPen selectionPen;
	if(mSelectionActive) { 
		selectionPen.setWidth(SELECTED_EDGE_WIDTH);
		selectionPen.setColor(QColor("cornflowerblue"));
	}else {
		selectionPen.setWidth(EDGE_WIDTH);
		selectionPen.setColor(QColor("white"));
	}
	painter.setPen(selectionPen);
	painter.drawRect(mSelectionRect);
}	

void ImageSelectionWidget::mousePressEvent(QMouseEvent* evt) {
	if(mSelectionRect.contains(evt->x(), evt->y())) {
		mSelectionActive = true;
	} else {
		mSelectionActive = false;
	}
	update();
}	
void ImageSelectionWidget::mouseReleaseEvent(QMouseEvent*) {
	mSelectionActive = false;
	update();
}
void ImageSelectionWidget::mouseMoveEvent(QMouseEvent* evt) {
	if(mSelectionActive == false) {
		mExpanding = false;
		mExpandingLeft = mExpandingRight = mExpandingTop = mExpandingBottom = false;
		setCursor(Qt::ArrowCursor);
		if(mSelectionRect.contains(evt->pos())) {
			QRect innerLeft = mSelectionRect.adjusted(RESIZE_THRESHHOLD,0,0,0);
			QRect innerTop = mSelectionRect.adjusted(0, RESIZE_THRESHHOLD, 0,0);
			QRect innerRight = mSelectionRect.adjusted(0,0,-RESIZE_THRESHHOLD, 0);
			QRect innerBottom = mSelectionRect.adjusted(0,0,0,-RESIZE_THRESHHOLD);
			if(innerLeft.contains(evt->pos())==false) {
				mExpandingLeft = true;
				setCursor(Qt::SizeHorCursor);
			} else if(innerTop.contains(evt->pos())==false) {
				mExpandingTop = true;
				setCursor(Qt::SizeVerCursor);
			} else if(innerRight.contains(evt->pos()) == false) {
				mExpandingRight = true;
				setCursor(Qt::SizeHorCursor);
			} else if(innerBottom.contains(evt->pos()) == false) {
				mExpandingBottom = true;
				setCursor(Qt::SizeVerCursor);
			}
		}

	}
	int dx, dy;
	dx = evt->x() - oldX;
	dy = evt->y() - oldY;
	if(mSelectionActive) {
		QRect baseSelection = mSelectionRect;
		if(mExpandingLeft) {
			mSelectionRect.adjust(dx, dx,-dx,-dx);
		} else if(mExpandingRight) {
			mSelectionRect.adjust(-dx,-dx,dx,dx);	
		} else if(mExpandingTop) {
			mSelectionRect.adjust(dy,dy,-dy,-dy);
		}else if(mExpandingBottom) {
			mSelectionRect.adjust(-dy,-dy,dy,dy);
		}else {
			int currX = mSelectionRect.center().x();
			int currY = mSelectionRect.center().y();
			mSelectionRect.moveCenter(QPoint(currX + dx, currY + dy));
		}
		if(mSelectionRect != baseSelection) {
			if(mPixmap.rect().contains(mSelectionRect) && mSelectionRect.width() >= 58 && mSelectionRect.height() >= 58) {
				emit selectionMoved(mSelectionRect.center().x(), mSelectionRect.center().y(), mSelectionRect.width()/2, mSelectionRect.height()/2);
			} else {
				mSelectionRect = baseSelection;
			}
		}
	}
	oldX = evt->x();
	oldY = evt->y();	
	update();
}

void ImageSelectionWidget::keyReleaseEvent(QKeyEvent*) {
	/*if(evt->key() == Qt::Key_Space) {
		grab();
	}*/
}

void ImageSelectionWidget::grab() {
	mGrabbedPixmap = mPixmap.copy(mSelectionRect); //scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).copy(mSelectionRect);
	emit grabbed();
}

/*--------------------------------------------------------------------------------*/
ImageSelectedWidget::ImageSelectedWidget(QWidget* parent):QWidget(parent) {
	initialize();
}
void ImageSelectedWidget::initialize() {
	mTimer.setDuration(700);
	mTimer.setUpdateInterval(10);
	mTimer.setCurveShape(QTimeLine::EaseInCurve);
	rotation= 0;
	mOpacity = 1;
	connect(&mTimer, SIGNAL(valueChanged(qreal)), this, SLOT(valueChanged(qreal)));
}

void ImageSelectedWidget::animate() {
	mGrabbedRect.translate(-mOrigRect.x(), -mOrigRect.y());
	mClippedRect.translate(-mOrigRect.x(), -mOrigRect.y());
	mTimer.setDirection(QTimeLine::Forward);
	mTimer.start();
}
void ImageSelectedWidget::animateReverse() {
	//mGrabbedRect.translate(-mOrigRect.x(), -mOrigRect.y());
	//mClippedRect.translate(-mOrigRect.x(), -mOrigRect.y());
	mTimer.setDirection(QTimeLine::Backward);
	mTimer.start();
}

void ImageSelectedWidget::valueChanged(qreal value) {
//	mOrigRect.moveTopLeft(QPoint(mOrigRect.x(), rect().height()*value));	
//	mClippedRect.moveTopLeft(QPoint(mGrabbedRect.x(), mGrabbedRect.y()+rect().height()*value));
	mOpacity = 1-value;
	update();
}
void ImageSelectedWidget::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::SmoothPixmapTransform|QPainter::Antialiasing);
	
	/*QLinearGradient g(0,0,size().width(), size().height());
	g.setColorAt(0, QColor("cornflowerblue"));
	g.setColorAt(1, QColor("khaki"));
	painter.setBrush(g);
	painter.drawRect(rect()); */

//	painter.save();
//	painter.rotate(rotation);
//	painter.setOpacity(.85);
	painter.setClipRegion(mGrabbedRect);	
	painter.drawPixmap(mGrabbedRect, mGrabbedPixmap);

	painter.setOpacity(mOpacity);
	QRegion r(rect());
	r = r.subtracted(mClippedRect);
	painter.setClipRegion(r);
	painter.drawPixmap(QPoint(0,0), mPixmap, mOrigRect);	


	painter.setBrush(QColor(0,0,0, CLIPED_OPACITY));
	painter.drawRect(0,0, mPixmap.width(), mPixmap.height());//mOrigRect);
	

//	painter.restore();
//	painter.setOpacity(1);

//	painter.setPen(Qt::NoPen);
//	painter.setBrush(QColor(0,0,0, 127));
//	painter.drawRect(mGrabbedRect.x()+10, mGrabbedRect.y()+10, mGrabbedRect.width(), mGrabbedRect.height());

}
void ImageSelectedWidget::setPixmap(const QPixmap& pixmap, const QRect& rect) {
	mPixmap = pixmap;
	mOrigRect = rect;
	//setGeometry(mOrigRect);
}
void ImageSelectedWidget::setGrabbedPixmap(const QPixmap& pixmap, const QRect& rect) {
	mGrabbedPixmap = pixmap;
	mGrabbedRect = rect;
	mClippedRect = mGrabbedRect;
}


/*------------------------------------------------------------*/
ImageSelectWidget::ImageSelectWidget(QWidget* parent):QStackedWidget(parent) {
	initialize();
}
void ImageSelectWidget::initialize() {
	mScrollArea.setFrameShape(QFrame::NoFrame);
	//mScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//mScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	mScrollArea.setWidget(&mSelectionWidget); 
	addWidget(&mScrollArea);
	addWidget(&mSelectedWidget);
	connect(&mSelectionWidget, SIGNAL(grabbed()), this, SLOT(doGrabbed()));
	connect(&mSelectionWidget, SIGNAL(selectionMoved(int,int, int,int)), this, SLOT(selectionMoved(int,int, int,int)));
	connect(&mSelectedWidget.mTimer, SIGNAL(finished()), this, SIGNAL(done()));
}

void ImageSelectWidget::setPixmap(const QPixmap& pixmap) {
	mSelectionWidget.setFixedSize(pixmap.size());
	mSelectionWidget.setPixmap(pixmap);
	setCurrentWidget(&mScrollArea);
}

QPixmap ImageSelectWidget::selection() {
	return mSelectionWidget.mGrabbedPixmap;
}

void ImageSelectWidget::selectionMoved(int centerX, int centerY, int extentX, int extentY) {
	mScrollArea.ensureVisible(centerX, centerY,extentX/2,extentY/2);
}

void ImageSelectWidget::grab() {
	mSelectionWidget.grab();
}
void ImageSelectWidget::keyReleaseEvent(QKeyEvent* evt) {
	if(evt->key() == Qt::Key_Space) {
		grab();
	}
}
void ImageSelectWidget::doGrabbed() {

	QRect inView(mScrollArea.horizontalScrollBar()->value()-1, mScrollArea.verticalScrollBar()->value()-1, mScrollArea.width(), mScrollArea.height());

	mSelectedWidget.setPixmap(mSelectionWidget.mPixmap,inView); ///mSelectionWidget.rect());
	mSelectedWidget.setGrabbedPixmap(mSelectionWidget.mGrabbedPixmap, mSelectionWidget.mSelectionRect);	

	mSelectedWidget.animate();
	setCurrentWidget(&mSelectedWidget);

	emit grabbed();
}
void ImageSelectWidget::tryAgain() {
	mSelectedWidget.animateReverse();
	connect(&mSelectedWidget.mTimer, SIGNAL(finished()), this, SLOT(goBack()));
	//setCurrentWidget(&mScrollArea);
}
void ImageSelectWidget::goBack() {
	disconnect(&mSelectedWidget.mTimer, SIGNAL(finished()), this, SLOT(goBack()));
	setCurrentWidget(&mScrollArea);
}


/*-------------------------------------------------------------*/

AvatarImageDialog::AvatarImageDialog(QWidget* parent):QWidget(parent) {
	initialize();
}
AvatarImageDialog::~AvatarImageDialog() {
	
}

void AvatarImageDialog::initialize() {
	setMinimumSize(640, 480);

	mSelectPushButton = new QPushButton();
	mSelectPushButton->setText("Click Here to Select an Image File ");
	mSelectPushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(mSelectPushButton, SIGNAL(clicked()), this, SLOT(selectAFile()));

	mPlusCropButton = new QPushButton();
	connect(mPlusCropButton, SIGNAL(clicked()), &mImageSelectWidget, SLOT(grab()));
	connect(&mImageSelectWidget, SIGNAL(grabbed()), this, SLOT(grabbed()));
	mImageSelectWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		

	mPlusCropButton->setText("Crop");
	mKludgeLayout.addWidget(mPlusCropButton);
	mPlusCropButtonKludge.setLayout(&mKludgeLayout);

	mPlusDoneButton = new QPushButton;
	mPlusDoneButton->setText("Finished");
	connect(mPlusDoneButton, SIGNAL(clicked()), this, SLOT(close()));
	mPlusTryAgainButton = new QPushButton;
	mPlusTryAgainButton->setText("Try Again");
	connect(mPlusTryAgainButton, SIGNAL(clicked()),this, SLOT(tryAgain()));

	mHPlusButtonsLayout.addWidget(mPlusTryAgainButton);
	mHPlusButtonsLayout.addWidget(mPlusDoneButton);
	mPlusYesNoButtons.setLayout(&mHPlusButtonsLayout);

	mButtonsSwitch.addWidget(&mPlusCropButtonKludge);
	mButtonsSwitch.addWidget(&mPlusYesNoButtons);
//	((QStackedWidget)mButtonsSwitch).setCurrentWidget(&mPlusCropButton);
	mButtonsSwitch.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
	
	mGPlusLayout.addWidget(new QLabel("Drag and Resize the Selection Box as Needed to Crop Your Image."), 0,0, 1,4);
	mGoBackPushButton = new QPushButton();
	mGoBackPushButton->setText("Select a Different File");
	connect(mGoBackPushButton, SIGNAL(clicked()), &mSwitch, SLOT(goBack())); 
	mGPlusLayout.addWidget(mGoBackPushButton, 1,0,1,4);
	mGPlusLayout.addWidget(&mImageSelectWidget, 2,0, 1, 4);
	mGPlusLayout.addWidget(&mButtonsSwitch, 3,0, 1,4);
	//mGPlusLayout.addWidget(&mPlusDone, 2,0);
	mImageSelectPlus.setLayout(&mGPlusLayout); 


	mSwitch.addWidget(mSelectPushButton);
	mSwitch.addWidget(&mImageSelectPlus);
	
	mGMainLayout.addWidget(&mSwitch,0,0);
	setLayout(&mGMainLayout);

	connect(&mImageSelectWidget, SIGNAL(done()), this, SIGNAL(done()));
}

QPixmap AvatarImageDialog::selection() {
	return mImageSelectWidget.selection();
}

void AvatarImageDialog::selectAFile() {
	QString file = QFileDialog::getOpenFileName(this, "Open Image", QDir::homePath() + "/Pictures/", "Image Files(*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm);;Any File Type(*.*)");	
	if(file.isEmpty()) return;
	QPixmap p(file);
	mImageSelectWidget.setPixmap(p);
	mSwitch.setCurrentWidget(&mImageSelectPlus, new SlideTransition(false, LEFT, 500));	
}

void AvatarImageDialog::grabbed() {
	mButtonsSwitch.setCurrentWidget(&mPlusYesNoButtons, new FadeTransition(1400));// SlideTransition(false, UP, 500));
}
void AvatarImageDialog::tryAgain() {
	//mButtonsSwitch.setCurrentWidget(&mPlusCropButtonKludge); //goBack();
	mButtonsSwitch.goBack();
	mImageSelectWidget.tryAgain();
}
