#include "PasswordChangeWidget.h"

PasswordChangeWidget::PasswordChangeWidget(QWidget* w):QWidget(w) {
	initialize();
}

void PasswordChangeWidget::initialize() {
	mVMainLayout = new QVBoxLayout();
	mFLayout = new QFormLayout();

	QFont f = mLabel.font();
	f.setPointSize(14);
	mLabel.setFont(f);
	mLabel.setText("Please Set Your Password to Something You'll Remember");
	mLabel.setWordWrap(true);
	mChangeButton.setText("Change My Password");
	mPasswordEdit.setEchoMode(QLineEdit::Password);
	mPWCheckEdit.setEchoMode(QLineEdit::Password);	

	mVMainLayout->addWidget(&mLabel);	
	mVMainLayout->addStretch();
	mFLayout->addRow("New Password:", &mPasswordEdit);
	mFLayout->addRow("Again to Double Check:", &mPWCheckEdit);
	mVMainLayout->addLayout(mFLayout);
	mVMainLayout->addStretch();
	mVMainLayout->addWidget(&mChangeButton);
	setLayout(mVMainLayout);

	connect(this, SIGNAL(controller_serverChangePassword(const QString&)), Controller::self(), SIGNAL(serverChangePassword(const QString&)));
	connect(&mChangeButton, SIGNAL(clicked()), this, SLOT(change()));
}

void PasswordChangeWidget::change() {
	bool okay = true;
	if(mPasswordEdit.text().size() < 5) {
		QMessageBox::information(this, "", "Password must be at least 5 characters.");	
		okay = false;
	}
	if(mPasswordEdit.text() != mPWCheckEdit.text()) {
		QMessageBox::information(this,"", "Passwords do not match.");	
		okay = false;
	}
	if(mPasswordEdit.text().startsWith("reset_")) { 
		QMessageBox::information(this,"", "Passwords can't start with 'reset_'.");
		okay = false;
	}
	if(mPasswordEdit.text().contains(" " )) {
		QMessageBox::information(this, "", "Passwords can't contains spaces.");
	}
	if(okay)
		emit controller_serverChangePassword(mPasswordEdit.text());
}
