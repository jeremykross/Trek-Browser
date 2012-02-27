#ifndef PANELSDOCK_H
#define PANELSDOCK_H

#define TRANSITION_TIME 1 //256 
#define DEFAULT_WIDTH 364 

#include <QObject>
#include <QDockWidget>
#include <QToolBar>
#include <QString>
#include <QStackedWidget>
#include <QPushButton>
#include <QAction>
#include <QActionGroup>
#include <QSize>
#include <QSizePolicy>
#include <QIcon>
#include <QTimeLine>
#include <QScrollArea>
#include <QList>
#include <QRect>
#include <QMessageBox>

#include <iostream>

#include "PanelWidget.h"

class PanelsDock : public QDockWidget {
	Q_OBJECT
	public:
		PanelsDock(const QString& title="", QWidget* parent=NULL, Qt::WindowFlags flags=NULL);
		void addPanel(PanelWidget* widget);	
		void openPanel(PanelWidget* widget);
		void setEnabled(PanelWidget* widget, bool enabled=true);
	signals:
		void panelOpened(PanelWidget* widget);
	protected slots:
		void setOpenPercentage(qreal value);
		void openStateChanged();
		void triggerStateChange();
	protected:
		void initialize();
		QAction* actionForPanel(PanelWidget* widget);
		QList<PanelWidget*> mPanels;
		QActionGroup* mActionGroup;
		QToolBar mToolBar;
		QStackedWidget mStackedWidget;

		QTimeLine mTimeLine;
		int mWidth;
		int mDoubleCheck;
		bool mIsOpen;
};	

#endif
