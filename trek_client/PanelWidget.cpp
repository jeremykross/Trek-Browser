#include "PanelWidget.h"

PanelWidget::PanelWidget(const QIcon& icon, const QString& title,QWidget* parent): QWidget(parent) {
	mIcon = icon;
	mTitle = title;	
	mMainWidget = NULL;
	mSearchable = false;
	mSearchLabel = NULL;
	initialize();
}

PanelWidget::~PanelWidget() {
	//FREE ME!
	if(mSearchable == false) {
		if(mSearchLabel) {
			 delete mSearchLabel;
			mSearchLabel = 	NULL;
		}
	}
}

void PanelWidget::initialize() {
	mClearButton.setText("x");
	mClearButton.setFixedWidth(20);
	connect(&mClearButton, SIGNAL(clicked()),&mSearchLine, SLOT(clear())); 
	mSearchLabel = new QLabel("Search:");
	mSearchLayout.addWidget(mSearchLabel);
	mSearchLayout.addWidget(&mSearchLine);
	mSearchLayout.addWidget(&mClearButton);
	QLabel* l = new QLabel(mTitle);
	l->setWordWrap(true);
	mPanelLayout.addWidget(l);
	setLayout(&mPanelLayout);
	setSearchable(mSearchable);
}

void PanelWidget::setSearchable(bool value) {
	mSearchable = value;
	if(mSearchable) {
		mPanelLayout.insertLayout(1, &mSearchLayout);	
	} else {
		mPanelLayout.removeItem(&mSearchLayout);	
	}	
}
bool PanelWidget::searchable() {
	return mSearchable;
}

QLineEdit* PanelWidget::searchLine() {
	return &mSearchLine;
}

QString PanelWidget::title() {
	return mTitle;
}
QIcon PanelWidget::icon() {
	return mIcon;
}

void PanelWidget::setWidget(QWidget* widget) {
	if(mMainWidget == NULL) {
		mMainWidget = widget;
		mMainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		mPanelLayout.addWidget(mMainWidget);
	}
	return;
}
QWidget* PanelWidget::widget() {
	return mMainWidget;	
}
