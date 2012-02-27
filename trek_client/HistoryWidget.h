#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
#include <QObject>
#include <QTreeView>
#include <QVBoxLayout>

#include <iostream>

#include "SiteListItemModel.h"
#include "SiteListItem.h"
#include "SiteListWidget.h"
#include "Controller.h"

class HistoryWidget: public SiteListWidget {
	Q_OBJECT
	public:
		HistoryWidget(UserData* userData=NULL, QWidget* parent=NULL);
		
};

#endif
