#include "PersonalMessageWidget.h"

PersonalMessageWidget::PersonalMessageWidget(UserData* userData, QWidget* parent):QWidget(parent) {
	mLocalData = mUserData = NULL;
	if(userData) setRemoteData(userData);
}

void PersonalMessageWidget::setRemoteData(UserData* userData) {
	mUserData = userData;
	initialize();
}
void PersonalMessageWidget::setLocalData(UserData* userData) {
	mLocalData = userData;
}

void PersonalMessageWidget::initialize() {
	setWindowTitle("Trek Personal Chat");
//	QFont f = mUsernameLabel.font();
//	f.setPointSize(11);
//	mUsernameLabel.setFont(f);
	
	mSendButton.setText("Send");
	mSendButton.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

	mConversationText.setReadOnly(true);

	mInputText.installEventFilter(this);

	QHBoxLayout* inputLayout = new QHBoxLayout();
	QWidget* inputWidget = new QWidget();
	inputLayout->addWidget(&mInputText);
	inputLayout->addWidget(&mSendButton);
	inputWidget->setLayout(inputLayout);
	QSplitter* splitter = new QSplitter();
	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(&mConversationText);
	splitter->addWidget(inputWidget);
	splitter->setCollapsible(0,false);
	splitter->setCollapsible(1,false);
	splitter->setHandleWidth(15);

	mToolbar.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
	mSaveAction = mToolbar.addAction("Save");
	mClearAction = mToolbar.addAction("Clear");

	mGMainLayout.addWidget(&mUserIconLabel,0,2,2,1,Qt::AlignRight);
	mGMainLayout.addWidget(&mToolbar, 1,0,1,2); //,1,2);
	mGMainLayout.addWidget(&mUsernameLabel,0,0,Qt::AlignLeft);
	mGMainLayout.addWidget(splitter, 2,0,1,3);

	setLayout(&mGMainLayout);

	QList<int> sizes;
	sizes.append(400);
	sizes.append(100);
	splitter->setSizes(sizes);

	userDataAltered();
	connect(mClearAction, SIGNAL(triggered()), &mConversationText, SLOT(clear()));
	connect(mSaveAction, SIGNAL(triggered()), this, SLOT(save())); 
	connect(&mSendButton, SIGNAL(clicked()), this, SLOT(send()));
	connect(mUserData, SIGNAL(dataAltered()), this, SLOT(userDataAltered()));
	connect(this, SIGNAL(controller_serverPersonalSpeak(const QUuid&, const QString&)), Controller::self(), SIGNAL(serverPersonalSpeak(const QUuid&, const QString&)));
	connect(Controller::self(), SIGNAL(personalCommuniqueReceived(const QUuid&, const QString&)), this, SLOT(controller_personalCommuniqueReceived(const QUuid&, const QString&)));


}

void PersonalMessageWidget::userDataAltered() {
	mUserIconLabel.setPixmap(mUserData->scaledIcon());
	mUsernameLabel.setText("Chatting with " +  mUserData->displayName()+":");
//	QPalette p(palette());
//	p.setColor(QPalette::Window, mUserData->usernameBGColor());
//	p.setColor(QPalette::WindowText, mUserData->usernameFontColor());
//	setPalette(p);
	update();
}

void PersonalMessageWidget::send() {
	QString mesg = mInputText.toPlainText();
	if(mesg.count() <= 0) return;
	mInputText.clear();
	logMessage(mLocalData, mesg);
	emit controller_serverPersonalSpeak(mUserData->guid(), mesg); 
}
void PersonalMessageWidget::save() {
	QString fileName = QFileDialog::getSaveFileName(this, "Save Conversation", QDir::homePath() + "/conversation.txt", "Text");		
	if(fileName.isEmpty()) return;
	QFile file(fileName);
	if(file.open(QFile::WriteOnly|QFile::Truncate)) {
		QTextStream out(&file);
		out << "Conversation with " << mUserData->displayName() << "\n";
		out << "Saved on " << QDate::currentDate().toString() << " at " << QTime::currentTime().toString() << "\n\n";
		out <<mConversationText.toPlainText(); 
	}
	file.close();
}

void PersonalMessageWidget::logMessage(UserData* userData, const QString& mesg) {
	mConversationText.moveCursor(QTextCursor::End);
	mConversationText.setTextBackgroundColor(userData->balloonBGColor());
	mConversationText.setTextColor(userData->balloonFontColor());
	QString userName = userData->displayName();
	userName.prepend(" ");
	userName.append(": ");
	mConversationText.insertPlainText(userName);	
	QString message = mesg;
	message.prepend(" ");
	message.append(" \n");
	mConversationText.insertPlainText(message);

	QScrollBar* scrollBar = mConversationText.verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
}

void PersonalMessageWidget::controller_personalCommuniqueReceived(const QUuid& guid, const QString& mesg) {
	if(guid == mUserData->guid()) {
		logMessage(mUserData, mesg);
		raise();
	}
}

bool PersonalMessageWidget::eventFilter(QObject* watched, QEvent* evt) {
	if(watched == &mInputText) {
		if(evt->type() == QEvent::KeyPress) {
			QKeyEvent* keyEvt = (QKeyEvent*)evt;	
			if(keyEvt->key() == Qt::Key_Return) {
				if((keyEvt->modifiers() &= Qt::ShiftModifier) == false) {
					send();
					return true;
				}
			}
		}
	}
	return false;
}
