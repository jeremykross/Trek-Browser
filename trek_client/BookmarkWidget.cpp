#include "BookmarkWidget.h"

BookmarkWidget::BookmarkWidget(UserData* userData, QWidget* parent):SiteListWidget(userData,parent) {
	SiteListWidgetConnection::setupBookmark(this);
}
