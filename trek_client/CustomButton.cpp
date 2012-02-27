#include "CustomButton.h"

CustomButton::CustomButton(QWidget* parent):QWidget(parent) {
	//mColor = Qt::blue;
	mColor = Qt::lightGray; //palette().color(QPalette::Window);
	mPressed = false;
	initialize();
}

void CustomButton::initialize() {
	mSizeHint.setWidth(42);
	mSizeHint.setHeight(42);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);	
}

QSize CustomButton::sizeHint() const {
	return mSizeHint;
}

void CustomButton::setPixmap(const QPixmap& pixmap) {
	mPixmap = pixmap;
	update();
}
void CustomButton::setText(const QString& text) {
	mText = text;
	update();
}
void CustomButton::setColor(const QColor& color) {
	mColor = color;
	update();
}
void CustomButton::setRandomColor() {
	srand(time(NULL));
	mColor.setRed(rand()%255);
	mColor.setGreen(rand()%255);
	mColor.setBlue(rand()%255);
}
QColor CustomButton::color() {
	return mColor;
}

void CustomButton::mousePressEvent(QMouseEvent* evt) {
	QWidget::mousePressEvent(evt);
	mPressed = true;
	update();
}
void CustomButton::mouseReleaseEvent(QMouseEvent* evt) {
	QWidget::mouseReleaseEvent(evt);
	mPressed = false;
	update();
	std::cout << "Emit clicked\n";
	emit clicked();
}

void CustomButton::setPixmapSize(int width, int height) {
	mPixmapRect.setWidth(width);
	mPixmapRect.setHeight(height);
	if(height > mSizeHint.height()-16) {
		mSizeHint.setHeight(height + 16);
	}
	if(width > mSizeHint.width()-16) {
		mSizeHint.setWidth(width+16);
	}
	update();
}

void CustomButton::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
	painter.setBrush(mColor);
	painter.setPen(Qt::black);//Qt::NoPen);
	painter.drawRoundedRect(rect(), 5,5);

	mPixmapRect.setRect(width()/2-mPixmapRect.width()/2,height()/2-mPixmapRect.height()/2, mPixmapRect.width(), mPixmapRect.height());

	painter.drawPixmap(mPixmapRect, mPixmap);
	//painter.drawPixmap(10,10, width()-20, height()-20, mPixmap,mPixmapRect.x(), mPixmapRect.y(), mPixmapRect.width(), mPixmapRect.height());
	if(mPressed == false) {
		QLinearGradient g(0,0,0, height());
		g.setColorAt(0, QColor(255,255,255,240));
		g.setColorAt(.1, QColor(255,255,255,225));
		g.setColorAt(.4, QColor(255,255,255,0));
		painter.setBrush(g);
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(5,5, width()-10, height()-10,5,5);
	} else {
		painter.setOpacity(.5);
		painter.setBrush(Qt::black);
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(rect(), 5,5);
	}

}
