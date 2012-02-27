#ifndef BOOKMARKWIDGET_H
#define BOOKMARKWIDGET_H

#include <QWidget>
#include <QObject>
#include <QTreeView>
#include <QVBoxLayout>

#include "SiteListWidget.h"
#include "SiteListItemModel.h"
#include "SiteListItem.h"
#include "Controller.h"
class EditableBookmarkWidget;

class BookmarkWidget:public SiteListWidget {
	Q_OBJECT
	public:
		BookmarkWidget(UserData* userData=NULL, QWidget* parent=NULL);

	friend class EditableBookmarkWidget;
};

#endif
