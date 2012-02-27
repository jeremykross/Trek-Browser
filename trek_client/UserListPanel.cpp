#include "UserListPanel.h"

UserListPanel::UserListPanel(QWidget* parent):QWidget(parent) {	
	mMouseX = mMouseY = 0;
	mMaxScale = DEFAULT_MAX_SCALE;
	mFallOff = DEFAULT_FALLOFF;
	mBaseScale = DEFAULT_SCALE;
	mBaseSize = DEFAULT_SIZE;
	mBaseBetween = DEFAULT_BETWEEN;

	connect(Controller::self(), SIGNAL(avatarAdded(UserData*)), this, SLOT(addUser(UserData*)));

}

void UserListPanel::addUser(UserData* userData) {
	mUserData.append(userData);
}
void UserListPanel::removeUser(UserData* userData) {
	mUserData.removeOne(userData);
}

void UserListPanel::drawIcon(QPainter* painter, UserData* userData) {	
	float halfDim = mBaseSize/2;
	painter->save();
	painter->translate(width()/2, 0);
	painter->drawPixmap(-halfDim,-halfDim,mBaseSize,mBaseSize, userData->icon());	
	painter->drawText(-painter->fontMetrics().width(userData->displayName())/2, halfDim+painter->fontMetrics().height(), userData->displayName());
	painter->restore();
}
void UserListPanel::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	QLinearGradient gradient(0,0,width(), height());
	gradient.setColorAt(0, Qt::gray);
	gradient.setColorAt(1, Qt::white);
	painter.fillRect(0,0,width(),height(),gradient);
	if(mUserData.size() > 0) {
		painter.translate(0,mBaseSize);
		drawIcon(&painter, mUserData[0]);
	}
}

void UserListPanel::enterEvent(QEvent*) {}
void UserListPanel::leaveEvent(QEvent*) {}
void UserListPanel::mouseMoveEvent(QMouseEvent*) {}
