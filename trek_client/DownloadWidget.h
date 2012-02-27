#ifndef DOWNLOADWIDGET_H
#define DOWNLOADWIDGET_H

#include <QWidget>
#include <QObject>
#include <QString>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include <QTableView> 
#include <QAbstractItemView>
#include <QHeaderView>
#include <QToolBar>
#include <QItemSelection>
#include <QAction>
#include <QModelIndex>
#include <QItemSelectionModel>

#include "Controller.h"
#include "DownloadModel.h"
#include "DownloadDelegate.h"

class DownloadWidget:public QWidget {
	Q_OBJECT
	public:	
		DownloadWidget();
		QSortFilterProxyModel* proxyModel();
	signals:
		void controller_downloadModelRequested();	
	protected slots:
		void controller_downloadModelReceived(DownloadModel* model);
		void selectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
		void clearSelectionTriggered(); 
		void stopSelectionTriggered(); 
		void clearAllTriggered(); 
	protected slots:
		void setupActions();
	protected:
		void initialize();
		QList<DownloadData*> mCurrentSelection;
		DownloadModel* mModel;	
		DownloadDelegate mDelegate;
		QSortFilterProxyModel mProxyModel;
		QVBoxLayout mVMainLayout;

		QAction* mReloadAction;
		QAction* mStopAction;
		QAction* mClearAction;
		QAction* mClearAllAction;
		 
		QToolBar mToolBar;
		QTableView mView;
};


#endif
