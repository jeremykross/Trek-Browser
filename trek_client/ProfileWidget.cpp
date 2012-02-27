#include "ProfileWidget.h"

ProfileWidget::ProfileWidget(QWidget* parent):QWidget(parent) {}

void ProfileWidget::setUserData(UserData* data) {
	mUserData = data;
	mEditable = false;
	initialize();

	connect(Controller::self(), SIGNAL(profileUpdated(const QUuid&,const QString&, const QString&, const QDate&, const QString&, const QString&, const QString&, const QString&)), this, SLOT(controller_profileUpdated(const QUuid&, const QString&, const QString&, const QDate&, const QString&, const QString&, const QString&, const QString&)));
	emit controller_serverRequestProfile(data->guid());
}

void ProfileWidget::setEditable(bool editable) {
	if(editable && !mEditable) {
		QPalette p(mEmailAddressLine.palette());
		p.setColor(QPalette::Base, QColor("cornflowerblue").lighter(150));

		mAboutMeText.setReadOnly(false);		
		mAboutMeText.setPalette(p);
		mOccupationText.setReadOnly(false);
		mOccupationText.setPalette(p);
		mHobbiesText.setReadOnly(false);
		mHobbiesText.setPalette(p);
		mFavoriteCultureText.setReadOnly(false);
		mFavoriteCultureText.setPalette(p);
		//mCalendarWidget.setVisible(true);
		connect(&mAboutMeText, SIGNAL(textChanged()),this, SLOT(enableApply()));	
		connect(&mOccupationText, SIGNAL(textChanged()), this, SLOT(enableApply()));	
		connect(&mHobbiesText, SIGNAL(textChanged()), this, SLOT(enableApply()));	
		connect(&mFavoriteCultureText, SIGNAL(textChanged()), this, SLOT(enableApply()));	

		mApplyButton.setVisible(true);
		mCancelButton.setVisible(true);
		mApplyButton.setEnabled(false);
	} else if(!editable && mEditable) {
		QPalette p(mUsernameLine.palette());
		mEmailAddressLine.setReadOnly(true);
		mEmailAddressLine.setPalette(p);

		mAboutMeText.setReadOnly(true);		
		mAboutMeText.setPalette(p);
		mOccupationText.setReadOnly(true);
		mOccupationText.setPalette(p);
		mHobbiesText.setReadOnly(true);
		mHobbiesText.setPalette(p);
		mFavoriteCultureText.setReadOnly(true);
		mFavoriteCultureText.setPalette(p);
	
		//mCalendarWidget.setVisible(false);
		mApplyButton.setVisible(false);
		mCancelButton.setVisible(false);
	}
	mEditable = editable;
}

void ProfileWidget::initialize() {
	mUsernameLine.setText(mUserData->displayName());
	mUsernameLine.setReadOnly(true);
	mEmailAddressLine.setReadOnly(true);
	mGenderLine.setReadOnly(true);
	mBirthDateLine.setReadOnly(true);
	mAboutMeText.setReadOnly(true);

	mFLayout.setRowWrapPolicy(QFormLayout::WrapAllRows);
	mFLayout.addRow("Username:", &mUsernameLine);
	//mFLayout.addRow("Email Address:", &mEmailAddressLine);
	mFLayout.addRow("Gender:", &mGenderLine);
	mFLayout.addRow("Birth Date:", &mBirthDateLine);
	//mFLayout.addRow(&mCalendarWidget);
	mFLayout.addRow("Education and Occupational Info:", &mOccupationText);
	mFLayout.addRow("Hobbies:", &mHobbiesText);
	mFLayout.addRow("Favorite Culture (i.e. Film, TV, Literature, Gaming):", &mFavoriteCultureText);
	mFLayout.addRow("Biography:", &mAboutMeText);			
	mFLayout.setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	
	mApplyButton.setText("Apply");
	mCancelButton.setText("Close");

	mGMainLayout.addLayout(&mFLayout,0,0,1,2);
	mGMainLayout.addWidget(&mApplyButton, 1,0);
	mGMainLayout.addWidget(&mCancelButton,1,1); 
	

	setLayout(&mGMainLayout);
	mEditable=true;
	setEditable(false);

	connect(&mCancelButton, SIGNAL(clicked()), this, SLOT(close()));
	connect(&mCancelButton, SIGNAL(clicked()), this, SIGNAL(canceled()));
	connect(&mApplyButton, SIGNAL(clicked()), this, SLOT(commit()));
	
	connect(this, SIGNAL(controller_serverUpdateProfile(const QString&, const QString&, const QString&, const QString&)), Controller::self(), SIGNAL(serverUpdateProfile(const QString&, const QString&, const QString&, const QString&)));
	connect(this, SIGNAL(controller_serverRequestProfile(const QUuid&)), Controller::self(), SIGNAL(serverRequestProfile(const QUuid&)));
//	connect(&mCalendarWidget, SIGNAL(clicked(const QDate&)), this, SLOT(setDate(const QDate&)));
}

void ProfileWidget::controller_profileUpdated(const QUuid& guid,const QString& emailAddress, const QString& sex, const QDate& birthDate, const QString& occupationInfo, const QString& hobbies, const QString& favoriteCulture, const QString& generalInfo) {		
	if(guid == mUserData->guid()) {
		mEmailAddressLine.setText(emailAddress);
		mGenderLine.setText(sex);
		QDate today = QDate::currentDate();
		int age = today.year() - birthDate.year();
		if(today.month() < birthDate.month() && today.day() < birthDate.day()) age--; 
		mBirthDateLine.setText(birthDate.toString("MMMM d yyyy") + " " +QString("(%1 years old)").arg(age));
		mOccupationText.setText(occupationInfo);
		mHobbiesText.setText(hobbies);
		mFavoriteCultureText.setText(favoriteCulture);
		mAboutMeText.setText(generalInfo);
	} 
	mApplyButton.setEnabled(false);
}

void ProfileWidget::enableApply() {
	mApplyButton.setEnabled(true);
}

void ProfileWidget::commit() {
	emit controller_serverUpdateProfile(mOccupationText.toPlainText(), mHobbiesText.toPlainText(), mFavoriteCultureText.toPlainText(), mAboutMeText.toPlainText()); 
	mApplyButton.setEnabled(false);
}
