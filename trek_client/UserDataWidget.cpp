#include "UserDataWidget.h"

UserDataWidget::UserDataWidget(QWidget* parent):QWidget(parent) {
	mData =NULL;
	mUserInfoWidget = NULL;
	mInfoWidget = NULL;
	mOldX = 0; mOldY = 0;
	mResizing = false;
}

QWidget* UserDataWidget::infoWidget() {
	return mInfoWidget;		
}

void UserDataWidget::mousePressEvent(QMouseEvent* evt) {
	if(evt->button() == Qt::LeftButton) {
		mOldX = evt->globalX();
		mOldY = evt->globalY();

		float distSquared = ((width()-evt->x())*(width()-evt->x()) + (height()-evt->y())*(height()-evt->y()));
		if( distSquared  < 24*24 ) {
			mResizing = true;	
			if(mInfoWidget) {
				mInfoWidget->hide();
				l.hide();
				mLocaleLabel.hide();
			}
		}

	}
}
void UserDataWidget::mouseReleaseEvent(QMouseEvent* evt) {
	if(evt->button() == Qt::LeftButton) {
		mOldX = mOldY = 0;
		if(mInfoWidget) {
			mLocaleLabel.show();
			mInfoWidget->show();
			l.show();
		}
		
		int xX = width() - XOFFSET;
		int xY = XOFFSET;
		//float distSquared = ((width()-evt->x())*(width()-evt->x()) + (height()-evt->y())*(height()-evt->y()));
		float distSquared = ((xX - evt->x())*(xX-evt->x()) + (xY-evt->y())*(xY-evt->y()));
		if(distSquared < 100) {
			close();
		}
		mResizing = false;
	}	
}
void UserDataWidget::mouseMoveEvent(QMouseEvent* evt) {
	if(mResizing) {
		//setGeometry(x(), y(), width()+dx, height()+dy);	
		resize(evt->x(), evt->y());
	} else {
		int dx = evt->globalX() - mOldX;
		int dy = evt->globalY() - mOldY;
		move(x() + dx, y() + dy);
	}
	mOldX = evt->globalX();
	mOldY = evt->globalY();

}

void UserDataWidget::resizeEvent(QResizeEvent*) {}

void UserDataWidget::paintEvent(QPaintEvent*) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);

	painter.setPen(Qt::NoPen);
	QBrush bg(QColor(0,0,0,225));
	painter.setBrush(bg);
	painter.drawRoundedRect(0,mUserInfoWidget->height(), width(), height()-mUserInfoWidget->height(), 5,5);

	QPen pen;
	pen.setColor(Qt::white);
	pen.setWidth(2);
	painter.setPen(pen);
	painter.drawLine(width()-20, height(), width(), height()-20);
	painter.drawLine(width()-14, height(), width(), height()-14);
	painter.drawLine(width()-8, height(), width(), height()-8);

	//Draw x at 10,10
	int offset = XOFFSET;
	painter.setBrush(bg);
	painter.drawEllipse(QPoint(width()-offset,offset), 10,10);
	painter.drawLine(width()-offset-5, offset-5, width()-offset+5, offset+5);
	painter.drawLine(width()-offset+5, offset-5, width()-offset-5, offset+5);

}

void UserDataWidget::closeEvent(QCloseEvent*) {
	emit closed(this);
}
void UserDataWidget::contextMenuEvent(QContextMenuEvent*) {
	/*QMenu menu;

	menu.addAction(mShowProfileAction);
	menu.addSeparator();
	menu.addAction(mShowFollowedAction);
	menu.addAction(mShowFansAction);
	//menu.addAction(mShowFriendsAction);
	menu.exec(QCursor::pos());*/
	emit controller_userContextMenuRequested(mData);
}

UserData* UserDataWidget::userData() {
	return mData;
}
void UserDataWidget::setUserData(UserData* data) {
	mData = data;
	mInfoWidget = NULL;
	initialize();
	//setupActions();
	resize(500, MINIMUMHEIGHT);
	userStatusAltered();
	connect(mData, SIGNAL(statusAltered()), this, SLOT(userStatusAltered()));
	connect(this, SIGNAL(controller_userContextMenuRequested(UserData*)), Controller::self(), SIGNAL(userContextMenuRequested(UserData*)));
}
void UserDataWidget::initialize() {		
	QPalette p(palette());
	p.setBrush(QPalette::Window, Qt::NoBrush);
	setPalette(p);
	setWindowFlags(Qt::FramelessWindowHint);
	setWindowIcon(mData->icon());
	setAttribute(Qt::WA_TranslucentBackground);

	mUserInfoWidget = new UserInfoWidget();
	mUserInfoWidget->setUserData(mData);
	mVMainLayout.addWidget(mUserInfoWidget);
	mVMainLayout.setAlignment(mUserInfoWidget, Qt::AlignTop);
	
	//mHLocaleLayout.addWidget(&mLocaleLabel);
	//mVMainLayout.addLayout(&mHLocaleLayout);
	mLocaleLabel.setWordWrap(true);
	mLocaleLabel.setMaximumWidth(512);
	mVMainLayout.addWidget(&mLocaleLabel);
	
	mLocaleLabel.hide();
	connect(&mLocaleLabel, SIGNAL(linkActivated(const QString&)), Controller::self(), SIGNAL(linkActivated(const QString&)));

	QPalette p1 = l.palette();
	p1.setBrush(QPalette::Foreground, Qt::white);
	l.setPalette(p1);
	mLocaleLabel.setPalette(p1);

	setLayout(&mVMainLayout);

	mInfoVisibilityTimeLine = new QTimeLine(HIDESHOWTIME, this);
	connect(mInfoVisibilityTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(visibilityChanged(qreal)));
	connect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(visibilityFinished()));

	connect(this, SIGNAL(controller_serverRequestStatus(const QUuid&)), Controller::self(), SIGNAL(serverRequestStatus(const QUuid&)));
}

void UserDataWidget::setupActions() {
	mShowProfileAction = new QAction("View Profile", this);
	mShowFollowedAction = new QAction("View Followed", this);
	mShowFansAction = new QAction("View Following", this);
	mShowFriendsAction = new QAction("View Friends", this);

	connect(mShowFollowedAction, SIGNAL(triggered()), this, SLOT(showFollowed()));
	connect(mShowFansAction, SIGNAL(triggered()), this, SLOT(showFans()));
	connect(mShowProfileAction, SIGNAL(triggered()), this, SLOT(showProfile()));
}

void UserDataWidget::showFans() {
	connect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupFans()));
	hideInfoWidget();
}

void UserDataWidget::showProfile() {
	connect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupProfile()));
	hideInfoWidget();
}
void UserDataWidget::showHistory() {
	connect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupHistory()));
	hideInfoWidget();

}
void UserDataWidget::showFavorites() {
	connect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupFavorites()));
	hideInfoWidget();

}
void UserDataWidget::clearInfo() {
	if(mInfoWidget) {
		mVMainLayout.removeWidget(mInfoWidget);
		mVMainLayout.removeWidget(&l);	
		delete mInfoWidget;
	}
}
void UserDataWidget::setInfo(const QString& labelText, QWidget* w) {
	QScrollArea* sw = new QScrollArea();
	QPalette p(w->palette());
	p.setColor(QPalette::WindowText, Qt::white);
	w->setPalette(p);
	l.setText("Viewing: " + labelText);
	sw->setWidget(w);
	sw->setWidgetResizable(true);
	mInfoWidget = sw;
	mVMainLayout.addWidget(&l);
	mVMainLayout.addWidget(mInfoWidget);
	l.hide();
	mLocaleLabel.hide();
	mInfoWidget->hide();

}
void UserDataWidget::setupHistory() {
	clearInfo();
	SiteListWidget* fw = new HistoryWidget();
	fw->setOwner(mData);
	setInfo("History:", fw);
	disconnect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupHistory()));
	showInfoWidget();
}
void UserDataWidget::setupFavorites() {
	clearInfo();
	SiteListWidget* fw = new BookmarkWidget();
	fw->setOwner(mData);
	setInfo("Favorites:", fw);
	disconnect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupFavorites()));
	showInfoWidget();
}
void UserDataWidget::setupProfile() {
	clearInfo();
	ProfileWidget* pw = new ProfileWidget();
	pw->setUserData(mData);
	setInfo("Profile:",pw);
	/*QScrollArea* w = new QScrollArea();
	ProfileWidget* pw = new ProfileWidget();
	QPalette p(pw->palette());
	p.setColor(QPalette::WindowText, Qt::white);
	pw->setPalette(p);
	pw->setUserData(mData);
	pw->setEditable(false);
	l.setText("Viewing Profile:");
	w->setWidget(pw);
	w->setWidgetResizable(true);
	mInfoWidget = w;
	mVMainLayout.addWidget(&l);
	mVMainLayout.addWidget(mInfoWidget);
	l.hide();
	mLocaleLabel.hide();
	mInfoWidget->hide();*/
	disconnect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupProfile()));
	showInfoWidget();
}
void UserDataWidget::setupFans() {
	if(mInfoWidget) {
		mVMainLayout.removeWidget(mInfoWidget);
		mVMainLayout.removeWidget(&l);	
		delete mInfoWidget;
	}
	RelationshipWidget* w = new RelationshipWidget();
	w->setUserData(mData);
	w->setViewFans();
	l.setText("Viewing Fans:");
	mInfoWidget = w;
	mVMainLayout.addWidget(&l);
	mVMainLayout.addWidget(mInfoWidget);
	l.hide();
	mLocaleLabel.hide();
	mInfoWidget->hide();
	disconnect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupFans()));
	showInfoWidget();
}

void UserDataWidget::userStatusAltered() {			
	if(mData->localeUrlAsString().isEmpty() == false) {
		mLocaleLabel.setText("Current Locale: <a href='" + mData->localeUrlAsString() + "'> " + mData->localeUrlAsString()+ "</a>");
	} else {
		mLocaleLabel.setVisible(false);	
	}
}

void UserDataWidget::showFollowed() {
	connect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupFollowed()));
	hideInfoWidget();
}
void UserDataWidget::setupFollowed() {
	if(mInfoWidget) {
		mVMainLayout.removeWidget(mInfoWidget);
		mVMainLayout.removeWidget(&l);	
		delete mInfoWidget;
	}
	RelationshipWidget* w = new RelationshipWidget();
	w->setUserData(mData);
	w->setViewFollowed();
	l.setText("Viewing Stars:");
	mInfoWidget = w;
	mVMainLayout.addWidget(&l);
	mVMainLayout.addWidget(mInfoWidget);
	l.hide();
	mLocaleLabel.hide();
	mInfoWidget->hide();
	disconnect(mInfoVisibilityTimeLine, SIGNAL(finished()), this, SLOT(setupFollowed()));
	showInfoWidget();
}

void UserDataWidget::visibilityChanged(qreal value) {
	setFixedHeight(MINIMUMHEIGHT + mSavedHeight*value);
}

void UserDataWidget::visibilityFinished() {
	if(mShowing) {
		setMinimumHeight(MINIMUMHEIGHT);
		setMaximumHeight(10000);
		resize(width(), mSavedHeight + MINIMUMHEIGHT);
		if(mInfoWidget) {
			setMinimumHeight(MINIMUMHEIGHT+100);
			l.setVisible(true);
			mLocaleLabel.setVisible(true);
			mInfoWidget->setVisible(true);
		}
	}else {
		setFixedHeight(MINIMUMHEIGHT);
	}
}

void UserDataWidget::showInfoWidget() {
	mInfoVisibilityTimeLine->stop();
	mInfoVisibilityTimeLine->setDirection(QTimeLine::Forward);
	mInfoVisibilityTimeLine->setCurrentTime(0);
	mInfoVisibilityTimeLine->start();
	mShowing = true;
}

void UserDataWidget::hideInfoWidget() {
	emit controller_serverRequestStatus(mData->guid());
	mLocaleLabel.hide();
	if(mInfoWidget) {
		l.hide();
		mInfoWidget->hide();
		mSavedHeight = height()-MINIMUMHEIGHT;
	} else {
		mSavedHeight = MINIMUMHEIGHT;	
	}
	//mSavedInfoHeight = mInfoWidget->height();
	mInfoVisibilityTimeLine->stop();
	mInfoVisibilityTimeLine->setDirection(QTimeLine::Backward);
	mInfoVisibilityTimeLine->setCurrentTime(mInfoVisibilityTimeLine->duration());
	mInfoVisibilityTimeLine->start();
	mShowing = false;
}
