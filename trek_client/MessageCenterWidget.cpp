#include "MessageCenterWidget.h"

MessageCenterWidget::MessageCenterWidget(QWidget* parent):QWidget(parent) {
	initialize();
}

void MessageCenterWidget::initialize() {	
	mTabWidget.setTabPosition(QTabWidget::West);
	//mTabWidget.addTab(&mFeedList, "Followed Feed");
	mTabWidget.addTab(&mChatsList, "Personal Chats");
	mTabWidget.addTab(&mExpeditionList, "Trek Requests"); 
	mVMainLayout.addWidget(&mTabWidget);
	setLayout(&mVMainLayout);

	connect(&mChatsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(chatDoubleClicked(QListWidgetItem*)));
	connect(&mExpeditionList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(expeditionDoubleClicked(QListWidgetItem*)));
}

void MessageCenterWidget::logExpeditionRequest(UserData* data) {
	QListWidgetItem* i = new QListWidgetItem();
	i->setForeground(QBrush(Qt::blue));
	i->setText(CURRENT_TIME + "Pending Browsing Trek w/ " + data->displayName());
	QVariant v;
	v.setValue(data);
	i->setData(Qt::UserRole, v);
	i->setData(Qt::UserRole+1, "pending");
	mExpeditionList.insertItem(0,i);

	mTabWidget.setCurrentWidget(&mExpeditionList);
}

void MessageCenterWidget::logExpeditionRequestResponse(UserData* data, bool accepted) {
	for(int i=0; i<mExpeditionList.count(); i++) {
		QListWidgetItem* item = mExpeditionList.item(i);
		QVariant v = item->data(Qt::UserRole);
		UserData* d = v.value<UserData*>();
		QString status = item->data(Qt::UserRole+1).toString();
		if(d == data && status.compare("done")) {
			if(accepted) {
				item->setForeground(QBrush(Qt::green));
				item->setText(CURRENT_TIME + "Accepted Trek w/ " + data->displayName());
			} else {
				item->setForeground(QBrush(Qt::red));
				item->setText(CURRENT_TIME + "Denied Trek w/ " + data->displayName());

			}
			item->setData(Qt::UserRole+1, "done");
		}
	}
}

void MessageCenterWidget::logPersonalChat(UserData* data) {
	QListWidgetItem* i = new QListWidgetItem();
	i->setForeground(QBrush(Qt::blue));
	i->setText("Personal Chat w/ " + data->displayName());
	QVariant v;
	v.setValue(data);
	i->setData(Qt::UserRole, v);
	mChatsList.insertItem(0,i);	
	mTabWidget.setCurrentWidget(&mChatsList);
}

void MessageCenterWidget::showChats() {
	mTabWidget.setCurrentWidget(&mChatsList);	
}
void MessageCenterWidget::showExpeditionRequests() {
	mTabWidget.setCurrentWidget(&mExpeditionList);
}

void MessageCenterWidget::chatDoubleClicked(QListWidgetItem* item) {
	QVariant v = item->data(Qt::UserRole);
	UserData* d = v.value<UserData*>();
	emit personalChatRequested(d);	
	std::cout << "Chat double clicked!\n";
}

void MessageCenterWidget::expeditionDoubleClicked(QListWidgetItem* item) {
	QString status = item->data(Qt::UserRole+1).toString();
	if(status.compare("pending") == 0) {
		QVariant v = item->data(Qt::UserRole);
		UserData* d = v.value<UserData*>();
		emit expeditionRequested(d);
	}
}
