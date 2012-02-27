#include "ExpeditionRequestWidget.h"

ExpeditionRequestWidget::ExpeditionRequestWidget(QWidget* parent):QWidget(parent) {
	initialize();	
}

void ExpeditionRequestWidget::initialize() {
	mVMainLayout = new QVBoxLayout();
	mUnaskedLayout = new QVBoxLayout();
	mAskedLayout = new QVBoxLayout();
	mSplitter = new QSplitter();

	mUnaskedView.setModel(&mUnaskedModel);
	mUnaskedView.setSelectionMode(QAbstractItemView::ExtendedSelection);
	mUnaskedView.setSpacing(2);
	mAskedView.setModel(&mAskedModel);	
	mAskedView.setSelectionMode(QAbstractItemView::ExtendedSelection);
	mAskedView.setSpacing(2);

	mUnaskedToAskedButton.setText("-->");
	mAskedToUnaskedButton.setText("<--");
	mInviteButton.setText("Invite / Uninvite");
	mUnaskedLayout->addWidget(new QLabel("Invitable Users"));
	mUnaskedLayout->addWidget(&mUnaskedView);
	mUnaskedLayout->addWidget(&mUnaskedToAskedButton);
	mAskedLayout->addWidget(new QLabel("Invited Users"));
	mAskedLayout->addWidget(&mAskedView);	
	mAskedLayout->addWidget(&mAskedToUnaskedButton);
	mUnaskedWidget.setLayout(mUnaskedLayout);
	mAskedWidget.setLayout(mAskedLayout);
	mSplitter->addWidget(&mUnaskedWidget);
	mSplitter->addWidget(&mAskedWidget);

	QLabel* l = new QLabel("Please Move the Users You'd Like to Invite into the Right Column and Users You'd Like to Uninvite (kick) Into the Left");
	l->setWordWrap(true);
	l->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
	mVMainLayout->addWidget(l);
	mColorLabel.setText("Choose a Color to Identify Your Party:");
	mColorLabel.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	mVMainLayout->addWidget(&mColorLabel);
	mColorButton.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	mColorButton.setRandomColor();
	mVMainLayout->addWidget(&mColorButton);
	mVMainLayout->addWidget(mSplitter);
	mVMainLayout->addWidget(&mInviteButton);

	setWindowTitle("Browsing Party Manager"); 
	setLayout(mVMainLayout);

	connect(&mUnaskedToAskedButton, SIGNAL(clicked()), this, SLOT(moveUnaskedToAsked()));
	connect(&mAskedToUnaskedButton, SIGNAL(clicked()), this, SLOT(moveAskedToUnasked()));
	connect(&mInviteButton, SIGNAL(clicked()), this, SLOT(invite()));

	connect(this, SIGNAL(controller_serverInviteToExpedition(const QColor&, QList<UserData*>)), Controller::self(), SIGNAL(serverInviteToExpedition(const QColor&, QList<UserData*>)));
	connect(this, SIGNAL(controller_serverUninviteFromExpedition(QList<UserData*>)), Controller::self(), SIGNAL(serverUninviteFromExpedition(QList<UserData*>)));

	connect(&mColorButton, SIGNAL(clicked()), this, SLOT(choosePartyColor()));

	mUnaskedModel.setRelationshipList(&mUnaskedList);
	mAskedModel.setRelationshipList(&mAskedList);	
}
void ExpeditionRequestWidget::addInvitableList(const QString& name, const QList<UserData*>& list) {
	mInvitableMap.insert(name, list);
}

void ExpeditionRequestWidget::setPartyColor(const QColor& c) {
	mColorButton.setColor(c);	
	mColorButton.hide();
	mColorLabel.hide();
}

void ExpeditionRequestWidget::setCurrentParty(const QList<UserData*>& list) {
	mCurrentParty = list;
	mAskedList = list;	

}

void ExpeditionRequestWidget::setInvitableList(const QString& name) {
	QList<UserData*> invitableList;
	if(name.compare("all") == 0) {
		QMap<QString, QList<UserData*> >::const_iterator i;
		for(i=mInvitableMap.constBegin(); i!=mInvitableMap.constEnd(); ++i) {
			QList<UserData*> list = i.value();
			for(int j=0; j<list.count(); j++) {
				if(invitableList.contains(list[j]) == false) {
					if(mAskedList.contains(list[j]) == false)
						invitableList.append(list[j]);	
				}
			}
		}
	}
	mUnaskedList = invitableList;
	mUnaskedModel.setRelationshipList(&mUnaskedList);
}

QList<UserData*> ExpeditionRequestWidget::helperMove(QListView* from, QListView* to) {
	QList<QModelIndex> selectedIndexes = from->selectionModel()->selection().indexes();
	RelationshipListModel* fromModel = (RelationshipListModel*)from->model();
	RelationshipListModel* toModel = (RelationshipListModel*)to->model();
	QList<UserData*> toBeMoved;
	for(int i=0; i<selectedIndexes.count(); i++) {
		UserData* d =fromModel->userDataFromIndex(selectedIndexes[i]);
		if(d) {
			toBeMoved.append(d);
		}
	}
	fromModel->removeUsers(toBeMoved);
	toModel->addUsers(toBeMoved);

	return toBeMoved;
}

void ExpeditionRequestWidget::moveUnaskedToAsked() {
	QList<UserData*> asked = helperMove(&mUnaskedView, &mAskedView);
	for(int i=0; i<asked.count(); i++) {
		if(mCurrentParty.contains(asked[i])==false) {
			mToInvite.append(asked[i]);
		}
		if(mToUninvite.contains(asked[i])) {
			mToUninvite.removeAll(asked[i]);
		}
	}
}
void ExpeditionRequestWidget::moveAskedToUnasked() {
	QList<UserData*> unasked = helperMove(&mAskedView, &mUnaskedView);
	for(int i=0; i<unasked.count(); i++) {
		if(mCurrentParty.contains(unasked[i])) {
			mToUninvite.append(unasked[i]);	
		}
		if(mToInvite.contains(unasked[i])) {
			mToInvite.removeAll(unasked[i]);
		}
	}
}

void ExpeditionRequestWidget::choosePartyColor() {
	QColor c= QColorDialog::getColor();
	if(c.isValid()) {
		mColorButton.setColor(c);
	}
}

void ExpeditionRequestWidget::invite() {
	if(mToInvite.count() > 0) {
		emit controller_serverInviteToExpedition(mColorButton.color(), mToInvite);
	} 
	if(mToUninvite.count() > 0) {
		emit controller_serverUninviteFromExpedition(mToUninvite);
	}
	close();	
}
