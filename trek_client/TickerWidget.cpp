#include "TickerWidget.h"

#define WINDOWS

TickerWidget::TickerWidget(QWidget* parent):QWidget(parent) {
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	//QFont font("Helvetica");
	QFont font;
	font.setPointSize(12);
	setFont(font);
	mPosX = width();

	mCurrWidth = 9001;
	mSwitched = false;
	mMessageDuration = 7000;

	mTimer.setInterval(mMessageDuration);
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(tick()));

	mNextTimeLine.setUpdateInterval(10);
	mNextTimeLine.setDuration(1000);
	connect(&mNextTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(nextProgress(qreal)));
};

void TickerWidget::setFont(const QFont& font) {
	mFont = font;
	QFontMetrics fm(mFont);
	setMinimumHeight(fm.height() + 20);	
	setMaximumHeight(minimumHeight());
}

QSize TickerWidget::sizeHint() {
	return QSize(128, 32);
}

void TickerWidget::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

	if(mNextTimeLine.state() == QTimeLine::Running) {
		painter.setTransform(mTransform);
	}

	painter.setBrush(QColor("black"));
	painter.setPen(Qt::white);
	QRect r= rect();
	r.setX(r.x() + 5);
	r.setY(r.y() + 5);
	r.setWidth(r.width() - 5);
	r.setHeight(r.height() -5);
	painter.setClipRect(r);
	painter.drawRoundedRect(r, 5,5);
	painter.setFont(mFont);
	painter.translate(width()/2-mCurrWidth/2, r.height()/2.0f + (painter.fontMetrics().height()/*+2*/)/2.0f);
	painter.setPen(Qt::yellow);
	painter.drawText(0,0,mCurrMessage);	
}
void TickerWidget::mouseReleaseEvent(QMouseEvent* evt) {
	emit clicked(mCurrMessage);
	if(evt->button() == Qt::LeftButton) {
		tick();
		mTimer.start();	
	}
}
void TickerWidget::changeMessage() {
	mCurrMessage = mMessageQueue.dequeue();
	QFontMetrics fm(mFont);
	mCurrWidth = fm.width(mCurrMessage);
}
void TickerWidget::nextProgress(qreal value) {
	if(value > .499f && mSwitched==false) {
		changeMessage();
		mSwitched = true;
	}
	mTransform.reset();
	if(mSwitched) {
		mTransform.translate(0, height());
		mTransform.scale(1,-1);
	}
	mTransform.translate(0, height()/2.0f);
	mTransform.rotate(-180*value, Qt::XAxis);
	mTransform.translate(0, -height()/2.0f);
	update();
}
void TickerWidget::tick() {
	if(mMessageQueue.count()) {
		if(isHidden()) { 
			emit shown();
			show();
		}
		nextMessage();
	} else {
		mCurrMessage = "";
		mTimer.stop();
		if(isVisible()) {
		hide();
			emit hidden();
		}
		emit done();
	}
}

bool TickerWidget::isShowingMessage() {
	return mCurrMessage.trimmed().length() || !mMessageQueue.isEmpty();
}
void TickerWidget::addMessage(const QString& mesg) {
	QString toEnqueue = mesg.trimmed();
	if(toEnqueue.startsWith("motd:")) {
		mMessageQueue.enqueue("Message of the Day:");
		toEnqueue = mesg.mid(6);
	}
	mMessageQueue.enqueue(toEnqueue);	
	if(mCurrMessage.isEmpty()) {
		tick();
		mTimer.start();
	}
}
void TickerWidget::start() {
	mTimer.start();
}
void TickerWidget::nextMessage() {
	mTransform.reset();
	mSwitched = false;
	mNextTimeLine.setCurrentTime(0);
	mNextTimeLine.start();
}
