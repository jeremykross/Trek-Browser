#include "AdminCommandWidget.h"

AdminCommandWidget::AdminCommandWidget(QWidget* w):QWidget(w) {
	mFMainLayout = NULL;
	initialize();
}

void AdminCommandWidget::initialize() {
	setWindowTitle("idgod 1");
	setMinimumWidth(256);

	mFMainLayout = new QFormLayout();
	
	mCommandCombo.addItem("Server Say");
	mCommandCombo.addItem("Motd");
	mCommandCombo.addItem("Kick");
	mCommandCombo.addItem("Block");
	mCommandCombo.addItem("Speak");
	mCommandCombo.addItem("Move To");

	mExecuteButton.setText("Execute");
	connect(&mExecuteButton, SIGNAL(clicked()), this, SLOT(execute()));

	mFMainLayout->addRow(new QLabel("Welcome to the Admin Tool."));
	mFMainLayout->addRow("Username:", &mUsernameLine);
	mFMainLayout->addRow("Command:", &mCommandCombo);
	mFMainLayout->addRow("Argument:", &mOptionLine);
	mFMainLayout->addRow(&mExecuteButton);

	setLayout(mFMainLayout);

	connect(&mOptionLine, SIGNAL(returnPressed()), this, SLOT(execute()));
	connect(this, SIGNAL(controller_serverExecuteAdminCommand(const QString&, int, const QString&)), Controller::self(), SIGNAL(serverExecuteAdminCommand(const QString&, int, const QString&)));
}

void AdminCommandWidget::execute() {
	emit controller_serverExecuteAdminCommand(mUsernameLine.text(), mCommandCombo.currentIndex(), mOptionLine.text()); 
	//mUsernameLine.clear();
	mOptionLine.clear();		
}
