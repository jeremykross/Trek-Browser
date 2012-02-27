#include "UserInfoWidget.h"

UserInfoWidget::UserInfoWidget(QWidget* parent):QWidget(parent) {
	mUserData = NULL;
	mStatus = "Offline";
}

void UserInfoWidget::initialize() {
	QPalette p(palette());
	p.setBrush(QPalette::Window, Qt::NoBrush);
	setPalette(p);
	setBackgroundRole(QPalette::Window);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	mIconBrush.setTexture(mUserData->scaledIcon());
	mIconBrush.setTransform(QTransform::fromTranslate(10,0));
	
	setMinimumWidth(200);
	setFixedHeight(PIXMAP_DIMENSION-PIXMAP_DIMENSION/1.75 + 50+25+5);

	mMousePressed = false;

	mMotdWidth = 0;
	mMotdTimer.setInterval(30);
	mMotdX = 0;
	//setMotd("Now is the time for all good men to come to the aid of their country.");
	connect(&mMotdTimer, SIGNAL(timeout()), this, SLOT(motdTick()));

	resize(500, height());

	connect(Controller::self(), SIGNAL(updateMotd(const QString&)), this, SLOT(setMotd(const QString&)));
	connect(this, SIGNAL(controller_serverRequestStatus(const QUuid&)), Controller::self(), SIGNAL(serverRequestStatus(const QUuid&)));
	connect(mUserData, SIGNAL(dataAltered()), this, SLOT(userDataAltered()));
	connect(mUserData, SIGNAL(statusAltered()), this, SLOT(userStatusAltered()));
}

void UserInfoWidget::setMotd(const QString& motd) {
	mMotd = motd;
	QFont f;
	QFontMetrics fm(f);
	mMotdWidth = fm.width(motd);
	mMotdTimer.start();
}
void UserInfoWidget::motdTick() {
//	if(mMotdX+5 > width()-mMotdWidth || mMotdX < 0 || mMousePressed == false )
	mMotdX-=1;
	if(mMotdX < -mMotdWidth) {
		mMotdX = width();
	}
	update();
}
void UserInfoWidget::userStatusAltered() {
	if(mUserData->status() == OFFLINE) {
		mStatus = "Offline";	
	} else if(mUserData->status() == ONLINE) {
		mStatus = "Online";
	}
	update();
}
void UserInfoWidget::userDataAltered() {
	setWindowIcon(mUserData->scaledIcon());
	mIconBrush.setTexture(mUserData->scaledIcon());
	update();
}

void UserInfoWidget::setUserData(UserData* userData) {
	mUserData = userData;
	initialize();
	emit controller_serverRequestStatus(userData->guid());
}
void UserInfoWidget::mousePressEvent(QMouseEvent* evt) {
	//mMousePressed = true;
	//mMotdTimer.stop();
	//mMotdTimer.setInterval(10);
	//mMotdTimer.start();
	QWidget::mousePressEvent(evt);
}
void UserInfoWidget::mouseReleaseEvent(QMouseEvent* evt) {
	//mMousePressed = false;
	//mMotdTimer.stop();
	//mMotdTimer.setInterval(50);
	//mMotdTimer.start();
	QWidget::mouseReleaseEvent(evt);
}
void UserInfoWidget::resizeEvent(QResizeEvent*) {
	mMotdX = width();
}
void UserInfoWidget::paintEvent(QPaintEvent* evt) {
	if(mUserData) {
		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

		painter.setClipRegion(evt->region());
	
		int nameY = PIXMAP_DIMENSION-PIXMAP_DIMENSION/1.75;
		int nameHeight = 50;

/*
		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0,0,0,127));
		painter.drawRoundedRect(10+PIXMAP_DIMENSION+5, nameY+nameHeight+3+5, width()-(10+PIXMAP_DIMENSION)*2-2, nameHeight/2, 2,2);

		painter.setPen(Qt::NoPen);
		painter.setBrush(QColor(0,0,0,127));
		painter.drawRoundedRect(0+5, nameY+5, width()-3, nameHeight,5,5);
*/
		//Draw the MessageBox
		QPen pen1(mUserData->balloonFontColor());
		pen1.setWidth(2);
		painter.setPen(pen1);
		painter.setBrush(QBrush(mUserData->balloonBGColor()));
		//painter.drawRoundedRect(10+PIXMAP_DIMENSION, nameY+nameHeight+3, width()-(10+PIXMAP_DIMENSION)*2, nameHeight/2, 2,2);
		QRectF motdRect = QRectF(2, nameY+nameHeight+3, width()-2, nameHeight/2.0f);
		painter.drawRoundedRect(motdRect, 2,2);
		painter.setClipRect(motdRect);	
		painter.drawText(motdRect.x()+mMotdX, painter.fontMetrics().height()/4.0f+motdRect.y()+motdRect.height()/2.0f+1, mMotd);


		painter.setClipping(false);
		//Draw the Usrname
		QPen p(mUserData->usernameFontColor());
		p.setWidth(3);
		painter.setPen(p);
		painter.setBrush(QBrush(mUserData->usernameBGColor()));
		painter.drawRoundedRect(3, nameY, width()-6, nameHeight,5,5);

	
		painter.setPen(p);
		//QFont f = painter.font();
		//f.setPointSize(14);
		//painter.setFont(f);
		QFont f = mUserData->font();
		f.setPointSize(12);
		painter.setFont(f);
		painter.drawText((width())/2.0f-painter.fontMetrics().width(mUserData->displayName())/2.0f, nameY + painter.fontMetrics().height()/2.0f + nameHeight/4.0f, mUserData->displayName());
		int oldHeight = painter.fontMetrics().height()+nameHeight/4;
		f.setPointSize(f.pointSize()-2);
		painter.setFont(f);
		painter.drawText((width())/2.0f-painter.fontMetrics().width(mStatus)/2.0f, nameY + painter.fontMetrics().height()/3.0f + oldHeight, mStatus);

		//Draw the Pixmap
		painter.setBrush(QBrush(QColor(0,0,0,127)));
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(15,5,PIXMAP_DIMENSION, PIXMAP_DIMENSION, 5,5);
		painter.setBrush(mIconBrush);
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(10,0,PIXMAP_DIMENSION, PIXMAP_DIMENSION, 5,5);

	}
}
