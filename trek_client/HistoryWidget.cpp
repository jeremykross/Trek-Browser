#include "HistoryWidget.h"

HistoryWidget::HistoryWidget(UserData* userData, QWidget* parent):SiteListWidget(userData, parent) {
	SiteListWidgetConnection::setupHistory(this);
}
