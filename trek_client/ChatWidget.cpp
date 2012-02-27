#include "ChatWidget.h"

ChatWidget::ChatWidget(QWidget* parent): QWidget(parent) {
	initialize();
}

ChatWidget::~ChatWidget() {}

void ChatWidget::initialize() {
	setWindowTitle("Chat Box");
	mainLayout = new QVBoxLayout();
	inputLayout = new QHBoxLayout();
	chatBox = new QTextBrowser();
	inputBox = new QLineEdit();
	
        //chatBox->setVisible(false);
	//chatBox->setTextInteractionFlags(Qt::TextBrowserInteraction);
	chatBox->setOpenLinks(false);
	chatBox->insertPlainText("Welcome To Chat!");
	//Setup the widgets
	chatBox->setReadOnly(true);
	connect(inputBox, SIGNAL(textChanged(const QString&)), this, SLOT(truncateMessage()));
	connect(inputBox, SIGNAL(returnPressed()), this, SLOT(sendMessage()));
	
	//Setup the layout
	inputLayout->addWidget(inputBox);
	mainLayout->addWidget(chatBox);
	mainLayout->addLayout(inputLayout);


        chatBox->hide();
	
	setLayout(mainLayout);
}

void ChatWidget::sendMessage() {
	if(inputBox->text().count()) {
		emit messageSent(inputBox->text());
		inputBox->clear();
	}
}
void ChatWidget::truncateMessage() {
	if(inputBox->text().count() > 255) {
		QMessageBox::information(this, "Sorry.", "Messages are limited to 255 characters!");
		QString text = inputBox->text();
		text.truncate(255);
		inputBox->setText(text);
	}
}

void ChatWidget::logEntrance(UserData* userData) {
	beginLog();
	chatBox->insertPlainText(userData->displayName() + " has entered the chat.");
	endLog();
}

void ChatWidget::logExit(UserData* userData, const QUrl& newUrl) {
	beginLog();
	if(newUrl.isValid())
		chatBox->insertHtml("<p>" + userData->displayName() + " navigated to <a href=\"" + newUrl.toString() +"\">" + newUrl.toString() + "</a>  </p>");
	else 
		chatBox->insertPlainText(userData->displayName() + " has left the building.");
	endLog();

}
void ChatWidget::logMessage(UserData* userData, const QString& mesg) {
	beginLog();

	chatBox->setTextBackgroundColor(userData->balloonBGColor());
	chatBox->setTextColor(userData->balloonFontColor());

	QString userName = userData->displayName();
	userName.prepend(" ");
	userName.append(": ");
	chatBox->insertPlainText(userName);

	QString message = mesg;
	message.prepend(" ");
	message.append(" ");
	chatBox->insertPlainText(message);
	endLog();
} 

void ChatWidget::reset(const QString& mRoomName) {
	chatBox->setTextBackgroundColor(Qt::white);
	chatBox->setTextColor(Qt::black);
	chatBox->clear();
	chatBox->insertHtml("Welcome to " + mRoomName + "!");
}

void ChatWidget::beginLog() {
	chatBox->moveCursor(QTextCursor::End);
	chatBox->setTextBackgroundColor(Qt::white);
	chatBox->setTextColor(Qt::black);
	chatBox->insertPlainText("\n");
}
void ChatWidget::endLog() {
	QScrollBar* scrollBar = chatBox->verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
}
QLineEdit* ChatWidget::lineEdit() {
	return inputBox;
}
QTextBrowser* ChatWidget::textEdit() {
	return chatBox;
}
void ChatWidget::setLogVisible(bool value) {
	chatBox->setVisible(value);
}

