#include "AvatarDesignWidget.h"

AvatarDesignWidget::AvatarDesignWidget(UserData* userData, QWidget* parent):QWidget(parent) {
	if(userData) setUserData(userData);
}

void AvatarDesignWidget::initialize() {
	setWindowTitle("Avatar Appearance Options");


//	QFont font;
//	font.setPointSize(14);
//	mUsernameLabel.setFont(font);
	mIconButton.setPixmap(mUserData->icon());
	mIconButton.setPixmapSize(96,96);
	//mUserInfoLayout.addWidget(&mUsernameLabel);
	//mUserInfoLayout.addWidget(&mIconButton);
	//mMainLayout.addLayout(&mUserInfoLayout);
	mGridLayout.addWidget(&mIconButton, 0,0,1,2);
//	mGridLayout.addWidget(&mImageDialog, 0,0,1,2);
	
	//setup the bottom layout
	mBGColorButton.setColor(mUserData->usernameBGColor());
//	mBGColorButton.setMaximumHeight(32);
	QLabel* l0 = new QLabel();
	l0->setText("Username Background Color:");
	l0->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	mGridLayout.addWidget(l0, 1,0);
	mGridLayout.addWidget(&mBGColorButton, 1,1);
//	mUsernameBGColorLayout.addWidget(l0);
//	mUsernameBGColorLayout.addWidget(&mBGColorButton);
//	mMainLayout.addLayout(&mUsernameBGColorLayout);
	

	mTextColorButton.setColor(mUserData->usernameFontColor());
	QLabel* l2 = new QLabel();
	l2->setText("Username Font Color:");
	l2->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	mGridLayout.addWidget(l2, 2,0);
	mGridLayout.addWidget(&mTextColorButton, 2,1);

	mSpeechBGColorButton.setColor(mUserData->balloonBGColor());
//	mSpeechBGColorButton.setMaximumHeight(32);
	QLabel* l1 = new QLabel();
	l1->setText("Speech Balloon Background Color:");
	l1->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	mGridLayout.addWidget(l1, 3,0);
	mGridLayout.addWidget(&mSpeechBGColorButton, 3,1);
//	mSpeechBGColorLayout.addWidget(l1);
//	mSpeechBGColorLayout.addWidget(&mSpeechBGColorButton);
//	mMainLayout.addLayout(&mSpeechBGColorLayout);


	mSpeechTextColorButton.setColor(mUserData->balloonFontColor());
	QLabel* l3 = new QLabel();
	l3->setText("Speech Balloon Font Color:");
	l3->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	mGridLayout.addWidget(l3, 4,0);
	mGridLayout.addWidget(&mSpeechTextColorButton, 4,1);
	

	setLayout(&mGridLayout);


	connect(&mIconButton, SIGNAL(clicked()), this, SLOT(chooseIcon()));

	connect(&mBGColorButton, SIGNAL(clicked()), this, SLOT(chooseUsernameBGColor()));
	connect(&mSpeechBGColorButton, SIGNAL(clicked()), this, SLOT(chooseBalloonBGColor()));
	connect(&mTextColorButton, SIGNAL(clicked()), this, SLOT(chooseUsernameFontColor()));
	connect(&mSpeechTextColorButton, SIGNAL(clicked()), this, SLOT(chooseBalloonFontColor()));

	connect(this, SIGNAL(updateUserData(UserData*)), Controller::self(), SIGNAL(serverUpdateAppearance(UserData*)));
}

void AvatarDesignWidget::setUserData(UserData* data) {
	mUserData = data;
	initialize();	
}

UserData* AvatarDesignWidget::userData() {
	return mUserData;
}

void AvatarDesignWidget::chooseIcon() {
	mImageDialog.setWindowTitle("Select and Crop");
	mImageDialog.setWindowFlags(Qt::WindowStaysOnTopHint);
	mImageDialog.show();
	connect(&mImageDialog, SIGNAL(done()), this, SLOT(updatePixmap()));
}

void AvatarDesignWidget::updatePixmap() {
	mIconButton.setPixmap(mImageDialog.selection());
	mUserData->setIcon(mImageDialog.selection()); 
//	mImageDialog.close();
}

void AvatarDesignWidget::chooseUsernameBGColor() {
	QColor c = QColorDialog::getColor(mUserData->usernameBGColor());
	if(c.isValid()) {
		if(sameColor(c, mTextColorButton.color())) return;
		mBGColorButton.setColor(c);
		mUserData->setUsernameBGColor(c);
	}
}
void AvatarDesignWidget::chooseBalloonBGColor() {
	QColor c = QColorDialog::getColor(mUserData->balloonBGColor());
	if(c.isValid()) {
		if(sameColor(c, mSpeechTextColorButton.color())) return;
		mSpeechBGColorButton.setColor(c);
		mUserData->setBalloonBGColor(c);
	}
}
void AvatarDesignWidget::chooseUsernameFontColor() {
	QColor c = QColorDialog::getColor(mUserData->usernameFontColor());
	if(c.isValid()) {
		if(sameColor(c, mBGColorButton.color())) return;
		mTextColorButton.setColor(c);
		mUserData->setUsernameFontColor(c);
	}
}
void AvatarDesignWidget::chooseBalloonFontColor() {
	QColor c = QColorDialog::getColor(mUserData->balloonFontColor());
	if(c.isValid()) {
		if(sameColor(c, mSpeechBGColorButton.color())) return;
		mSpeechTextColorButton.setColor(c);
		mUserData->setBalloonFontColor(c);
	}
}

bool AvatarDesignWidget::sameColor(const QColor& one, const QColor& two) {
	if(one == two) {
		QMessageBox::information(NULL, "Sorry", "You can't choose the same background and font colors.  If you're confused about what your avatar will look like click 'Test My Avatar.'");
		return true;
	}
	return false;
}

void AvatarDesignWidget::commit() {
	//Need to be connected and authenticated!
	emit updateUserData(mUserData);
	mUserData->altered();
}

void AvatarDesignWidget::closeEvent(QCloseEvent*) {
	commit();
}

AvatarTestWidget::AvatarTestWidget(UserData* userData, QWidget* parent):QWidget(parent) {
	mUserData = userData;
	initialize();
}

void AvatarTestWidget::initialize() {
	mAvatar = new GraphicsAvatarItem(mUserData);
	mSpeechBalloon = new GraphicsSpeechBalloonItem(mAvatar, mUserData);	
//	mGraphicsScene.setSceneRect(QRect(0,0,100,100));
//	mGraphicsView.setSceneRect(QRect(0,0,100,100));
	mGraphicsView.setScene(&mGraphicsScene);
	mGraphicsView.setBackgroundRole(QPalette::Window);
	mGraphicsScene.addItem(mAvatar);
//	mAvatar->setPos(50,50);

	mDoneButton.setText("Done");
	mGMainLayout.addWidget(&mGraphicsView,0,0, 1,2);
	mGMainLayout.addWidget(new QLabel("Type Here and Hit Enter:"),1,0);
	mGMainLayout.addWidget(&mTextBox, 1,1);
//	mGMainLayout.addWidget(&mDoneButton, 2,0);

	connect(&mTextBox, SIGNAL(returnPressed()), this, SLOT(speak()));

	setLayout(&mGMainLayout);
}

void AvatarTestWidget::speak() {
	mAvatar->speak(mTextBox.text());
	mTextBox.clear();
}

void AvatarTestWidget::setUserData(UserData* userData) {
	if(mAvatar) {
		mGraphicsScene.removeItem(mAvatar);
		delete mSpeechBalloon;
		delete mAvatar;
	}
	mUserData = userData;
	mAvatar = new GraphicsAvatarItem(mUserData);
	mSpeechBalloon = new GraphicsSpeechBalloonItem(mAvatar, mUserData);

	mGraphicsScene.addItem(mAvatar);

}

AvatarDesignAndTestWidget::AvatarDesignAndTestWidget(UserData* userData, QWidget* parent):QWidget(parent) {
	mUserData = userData;
	mDesign.setUserData(mUserData);
	initialize();
}

AvatarDesignWidget* AvatarDesignAndTestWidget::designWidget() { return &mDesign; }
AvatarTestWidget* AvatarDesignAndTestWidget::testWidget() { return &mTest; }

void AvatarDesignAndTestWidget::initialize() {
	mSwitch.addWidget(&mDesign);
	mSwitch.addWidget(&mTest);
	mSwitchButton.setText("Test My Avatar");
	mSwitchButton.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	mIsDesign = true;

	mGMainLayout = new QGridLayout();
	mGMainLayout->addWidget(&mSwitch,0,0);
	mGMainLayout->addWidget(&mSwitchButton,1,0,2,1);

	connect(&mSwitchButton, SIGNAL(clicked()), this, SLOT(switchEm()));

	setLayout(mGMainLayout);
}
void AvatarDesignAndTestWidget::switchEm() {
	if(mIsDesign) {
		mTest.setUserData(mDesign.userData());
		mSwitch.setCurrentWidget(&mTest, new SlideTransition(false, LEFT, 500));
		mSwitchButton.setText("Go Back");
	} else {
		//mSwitch.setCurrentWidget(&mDesign, new SlideTransition(false, RIGHT, 500));
		mSwitch.goBack();
		mSwitchButton.setText("Test My Avatar");
	}	
	mIsDesign = !mIsDesign;
}

