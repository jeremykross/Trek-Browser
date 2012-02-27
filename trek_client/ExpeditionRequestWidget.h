#ifndef EXPEDITIONREQUESTWIDGET_H
#define EXPEDITIONREQUESTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QGridLayout>
#include <QListView>
#include <QPushButton>
#include <QSplitter>
#include <QLabel>
#include <QSizePolicy>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QAbstractItemView>
#include <QColor>
#include <QColorDialog>

#include "UserData.h"
#include "UserDataFactory.h"
#include "RelationshipModel.h"
#include "CustomButton.h"
#include "Controller.h"

class ExpeditionRequestWidget:public QWidget {
	Q_OBJECT
	public:
		ExpeditionRequestWidget(QWidget* parent=NULL);
		~ExpeditionRequestWidget() {}
		void addInvitableList(const QString& name, const QList<UserData*>& list);
		void setCurrentParty(const QList<UserData*>& list);
		void setInvitableList(const QString& name);
		void setPartyColor(const QColor& c);
	signals:
		void controller_serverInviteToExpedition(const QColor& c, QList<UserData*> list);
		void controller_serverUninviteFromExpedition(QList<UserData*> list);

	protected slots:
		void moveUnaskedToAsked();
		void moveAskedToUnasked();
		void invite();
		void choosePartyColor();
	protected:
		void initialize();
		QList<UserData*> helperMove(QListView* from, QListView* to);
	
		QVBoxLayout* mVMainLayout; 
		QVBoxLayout* mUnaskedLayout;
		QVBoxLayout* mAskedLayout;
		QLabel mColorLabel;
		CustomButton mColorButton;
		QWidget mUnaskedWidget;
		QWidget mAskedWidget;
		QSplitter* mSplitter;
		QListView mUnaskedView;
		QListView mAskedView;
		QPushButton mUnaskedToAskedButton;
		QPushButton mAskedToUnaskedButton;
		QPushButton mInviteButton;
		RelationshipListModel mUnaskedModel;
		RelationshipListModel mAskedModel;
	
		QList<UserData*> mCurrentParty;
		QList<UserData*> mUnaskedList;
		QList<UserData*> mToInvite;
		QList<UserData*> mToUninvite;
		QList<UserData*> mAskedList;

		QMap<QString, QList<UserData*> > mInvitableMap;

};

#endif
