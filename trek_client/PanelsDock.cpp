#include "PanelsDock.h"

PanelsDock::PanelsDock(const QString& title, QWidget* parent, Qt::WindowFlags flags):QDockWidget(title, parent, flags) {
	mActionGroup = NULL;
	initialize();
}

void PanelsDock::initialize() {
	mActionGroup = new QActionGroup(this);
	mActionGroup->setExclusive(false);
	//mActionGroup->setExclusive(true);
	setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
	mToolBar.setParent(this);
	mStackedWidget.setParent(this);
	mToolBar.setOrientation(Qt::Vertical);
	mToolBar.setIconSize(QSize(20,20));
	//QAction* aAction = mToolBar.addAction("Downloads");
	//aAction->setIcon(QIcon("/home/jkross/Pictures/icons/download.png"));
	//connect(aAction, SIGNAL(triggered()), this, SLOT(aTriggered()));
	//mToolBar.addAction("Users")->setIcon(QIcon("/home/jkross/Pictures/icons/users.png"));;
	setTitleBarWidget(&mToolBar);
	setWidget(&mStackedWidget);
	
	mStackedWidget.setMinimumWidth(0);
	mStackedWidget.setMaximumWidth(0);
	
	mTimeLine.setCurveShape(QTimeLine::LinearCurve);
	mTimeLine.setDuration(TRANSITION_TIME);
	//mTimeLine.setFrameRange(0,20);
	mTimeLine.setUpdateInterval(10);
	connect(&mTimeLine, SIGNAL(valueChanged(qreal)), this, SLOT(setOpenPercentage(qreal)));
	connect(&mTimeLine, SIGNAL(finished()), this, SLOT(openStateChanged()));
	mIsOpen = false;
	mWidth = DEFAULT_WIDTH;
}

void PanelsDock::addPanel(PanelWidget* widget) {
	if(widget == NULL) return;
	mStackedWidget.addWidget(widget);
	QAction* newAction = mToolBar.addAction(widget->title());
	newAction->setIcon(widget->icon());
	newAction->setCheckable(true);
	mActionGroup->addAction(newAction);
	QVariant v;
	v.setValue(widget);
	newAction->setData(v);//QVariant().setValue(widget));
	connect(newAction, SIGNAL(triggered()), this, SLOT(triggerStateChange()));

}

QAction* PanelsDock::actionForPanel(PanelWidget* widget) {
	for(int i=0; i<mActionGroup->actions().count(); i++) {
		QAction* action = mActionGroup->actions()[i];
		PanelWidget* currWidget  = action->data().value<PanelWidget*>();
		if(widget == currWidget) {
			return action;
			/*if(action->isChecked() == false) {
				action->trigger();
			}
			break;*/
		}
	}
	return NULL;

}

void PanelsDock::openPanel(PanelWidget* widget) {
	QAction* action = actionForPanel(widget);
	if(action == NULL) return;
	if(action->isChecked() == false) {
		action->trigger();
	}
}

void PanelsDock::setEnabled(PanelWidget* widget, bool enabled) {
	QAction* action  = actionForPanel(widget);	
	if(action == NULL) return;
	action->setEnabled(enabled);
}

void PanelsDock::setOpenPercentage(qreal value) {
	if(mIsOpen) {
		mStackedWidget.setFixedWidth(value*mWidth);
	} else {
		mStackedWidget.setFixedWidth(value*mWidth);
	}
}
void PanelsDock::openStateChanged() {
	if(mIsOpen) {
		//mStackedWidget.setMinimumWidth(0);
		mStackedWidget.setMaximumWidth(QWIDGETSIZE_MAX);	
		mStackedWidget.resize(mWidth, mStackedWidget.height());
		update();
		//mStackedWidget.resize(mWidth, height()); //setGeometry(geom.x(), geom.y(), mWidth, geom.height());
	} else {
		mStackedWidget.setMinimumWidth(0);
		mStackedWidget.setMaximumWidth(0);	
		mStackedWidget.resize(0, height());
	//	mStackedWidget.setGeometry(geom.x(), geom.y(),0, geom.height());
	}
}
void PanelsDock::triggerStateChange() {
	QAction* actionResponsible = (QAction*)sender();

	mTimeLine.stop();
	
	if(mIsOpen) mWidth = DEFAULT_WIDTH;//mStackedWidget.width();
	//mDoubleCheck = mWidth;

	for(int i=0; i<mActionGroup->actions().count(); i++) {
		//Uncheck all the actions
		mActionGroup->actions()[i]->setChecked(false);
	}
	QVariant v = actionResponsible->data();
	PanelWidget* newWidget  = actionResponsible->data().value<PanelWidget*>();
	PanelWidget* currWidget = (PanelWidget*)mStackedWidget.currentWidget();
	if(newWidget != currWidget) {
		//Check action responsible
		actionResponsible->setChecked(true);
		if(mIsOpen) {
			emit panelOpened(newWidget);
			//Close and reopen
			mStackedWidget.setCurrentWidget(newWidget);
			return;
		} else {
			mStackedWidget.setCurrentWidget(newWidget);
		}
	}
	mTimeLine.stop();
	if(mIsOpen) {
		mTimeLine.setDirection(QTimeLine::Backward);
		mTimeLine.setCurrentTime(TRANSITION_TIME);
	} else {
		emit panelOpened(newWidget);
		actionResponsible->setChecked(true);
		mTimeLine.setDirection(QTimeLine::Forward);
		mTimeLine.setCurrentTime(0);
	}
	mTimeLine.start();
	mIsOpen = !mIsOpen;	
}

