#include "PasswordResetWidget.h"

PasswordResetWidget::PasswordResetWidget(QWidget* parent):QWidget(parent) {
	initialize();
}

void PasswordResetWidget::initialize() {
	setFocusPolicy(Qt::StrongFocus);

	mVMainLayout = new QVBoxLayout();
	mHButtonLayout = new QHBoxLayout();

	QFont f = mLabel.font();
	f.setPointSize(14);
	mLabel.setFont(f);
	mLabel.setText("The Password You Typed is Incorrect.\n\nIf you've forgotten your password, we can send you a new one at the given email address.");
	mLabel.setWordWrap(true);

	mResetButton.setText("Yes, Please Reset It.");
	mCancelButton.setText("No, Let Me Try Again.");

	mHButtonLayout->addWidget(&mResetButton);
	mHButtonLayout->addWidget(&mCancelButton);

	mVMainLayout->addWidget(&mLabel);
	mVMainLayout->addStretch();
	mVMainLayout->addLayout(mHButtonLayout);
	setLayout(mVMainLayout);

	connect(this, SIGNAL(controller_serverRequestPasswordReset()), Controller::self(), SIGNAL(serverRequestPasswordReset()));
	connect(&mResetButton, SIGNAL(clicked()), this, SIGNAL(controller_serverRequestPasswordReset()));
	connect(&mCancelButton, SIGNAL(clicked()), this, SIGNAL(canceled()));
}

void PasswordResetWidget::keyPressEvent(QKeyEvent* evt) {
	std::cout << "key event\n";
	if(evt->key() == Qt::Key_Escape) {
		emit canceled();
	}
	QWidget::keyPressEvent(evt);
}
