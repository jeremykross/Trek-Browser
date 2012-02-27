#include "AddressComboBox.h"

AddressLineEdit::AddressLineEdit(QWidget* parent):QLineEdit(parent) {
}

void AddressLineEdit::setFavoriteIcon(const QIcon& icon) {
	mFavoriteIcon = icon;	
	mRight = 20;
	setupMargins();
}

void AddressLineEdit::setFavicon(const QIcon& icon) {
	mFavicon = icon;	
	if(mFavicon.isNull() == false)
		mLeft = 20;
	else
		mLeft = 0;
	setupMargins();
}
void AddressLineEdit::clearFavicon() {
	mFavicon = QIcon();
	mLeft = 0;
	setupMargins();
}
void AddressLineEdit::setupMargins() {
	setTextMargins(mLeft, 0, mRight, 0);
}
void AddressLineEdit::mouseMoveEvent(QMouseEvent* evt) {
	QLineEdit::mouseMoveEvent(evt);
	QRect r(rect().width()-20,0,20,rect().height());
	if(r.contains(evt->x(), evt->y())) {
		setCursor(Qt::PointingHandCursor);
	} else {
		setCursor(Qt::IBeamCursor);	
	}
}
void AddressLineEdit::mouseReleaseEvent(QMouseEvent* evt) {
	QLineEdit::mouseReleaseEvent(evt);
	QRect r(rect().width()-20, 0, 20, rect().height());
	if(r.contains(evt->x(), evt->y())) {
		emit favoriteIconClicked();
	}
}
void AddressLineEdit::leaveEvent(QEvent* evt) {
	unsetCursor();
	QLineEdit::leaveEvent(evt);
}
void AddressLineEdit::paintEvent(QPaintEvent* evt) {
	QLineEdit::paintEvent(evt);
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor("cornflowerblue").darker(105));

	painter.setOpacity(.55);
	if(percentComplete > 0 && hasFocus() == false)
		painter.drawRect(rect().x(),rect().y(), rect().width()*((float)percentComplete/100), rect().height());
	//Draw the icon
	painter.setOpacity(1);
	if(mFavicon.isNull() == false) {
		mFavicon.paint(&painter, 0,0, 20, rect().height());
	}
	if(mFavoriteIcon.isNull() == false) {
		mFavoriteIcon.paint(&painter, rect().width()-20, 0, 20, rect().height());			
	}
	
}

void AddressLineEdit::setPercentComplete(int value) {
	percentComplete = value;
	//filledRect.setRect(0, 0, width()*(value/100), height());
}

AddressComboBox::AddressComboBox(QWidget* parent): QComboBox(parent) {
	setStyleSheet("QComboBox:editable {margin:4px; border: 2px groove gray;border-radius:5px;padding: 2px 2px; font-size:14px}\
			QComboBox::drop-down {top:100; width:0px; height:0px;}\
			QComboBox::drop-down:editable {width:0px; height:0px;}");
	setEditable(true);
	setLineEdit(&customLineEdit);

	setPercentComplete(50);
	setCompleter(NULL);	
}

AddressLineEdit* AddressComboBox::addressLineEdit() {
	return &customLineEdit; 
}

void AddressComboBox::setPercentComplete(int value) {
	customLineEdit.setPercentComplete(value);
	customLineEdit.update();
}

void AddressComboBox::paintEvent(QPaintEvent* evt) {
	QComboBox::paintEvent(evt);
}



