#include "UserSelectionsWidget.h"

UserSelectionsWidget::UserSelectionsWidget(UserData* d, QWidget* parent):QWidget(parent) {
	mUserData = d;
	initialize();
}

void UserSelectionsWidget::initialize() {
	mHMainLayout = new QHBoxLayout();

	mProfileButton.setText("View Profile");
	mHMainLayout->addWidget(&mProfileButton);
	mChatButton.setText("Private Chat");
	mHMainLayout->addWidget(&mChatButton);
	mLikeButton.setText("Become a Fan");
	mHMainLayout->addWidget(&mLikeButton);	
	connect(&mChatButton, SIGNAL(clicked()), this, SLOT(selectionMade()));
	connect(&mProfileButton, SIGNAL(clicked()), this, SLOT(selectionMade()));
	connect(&mLikeButton, SIGNAL(clicked()), this, SLOT(selectionMade()));


	mHMainLayout->setContentsMargins(1,1,1,1);

	setLayout(mHMainLayout);

	QPalette p = palette();
	p.setColor(QPalette::Window, QColor(255,255,255,0));
	setPalette(p);
}

void UserSelectionsWidget::selectionMade() {
	if(sender() == &mChatButton) {
		emit privateChat(mUserData);	
	} else if(sender() == &mProfileButton) {
		emit profile(mUserData);
	} else if(sender() == &mLikeButton) {
		emit toggleLiked(mUserData);
	}
	return;
}
