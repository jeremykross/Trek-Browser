#ifndef AVATARDESIGNWIDGET_H
#define AVATARDESIGNWIDGET_H

#include <QWidget>
#include <QObject>

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QColorDialog>
#include <QColor>
#include <QPalette>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QMessageBox>
#include <QLineEdit>
#include <QPalette>
#include <QSizePolicy>
#include <QCloseEvent>

#include <QGraphicsView>
#include <QGraphicsScene>


#include "Controller.h"
#include "UserData.h"
#include "SwitchWidget.h"
#include "SlideTransition.h"
#include "CustomButton.h"
#include "AvatarImageDialog.h"

#include "GraphicsAvatarItem.h"
#include "GraphicsSpeechBalloonItem.h"


class AvatarDesignWidget: public QWidget {
	Q_OBJECT;
	public:
		AvatarDesignWidget(UserData* userData=NULL, QWidget* parent=NULL);
		UserData* userData();
		void setUserData(UserData* userData);
	protected:
		void closeEvent(QCloseEvent* evt);
	signals:
		void updateUserData(UserData* userData);
		void dataChanged();
	public slots:
		void commit();
	protected slots:
		void chooseIcon();
		void chooseUsernameBGColor();
		void chooseBalloonBGColor();
		void chooseUsernameFontColor();
		void chooseBalloonFontColor();

		bool sameColor(const QColor& one, const QColor& two);

		void updatePixmap();
	protected:
		void initialize();

		UserData* mUserData;

		QGridLayout mGridLayout;

		SwitchWidget mTestSwitch;

		CustomButton mIconButton;
		CustomButton mBGColorButton;
		CustomButton mTextColorButton;
		CustomButton mSpeechBGColorButton;
		CustomButton mSpeechTextColorButton;

		AvatarImageDialog mImageDialog;
};

class AvatarTestWidget:public QWidget {
	Q_OBJECT;
	public:
		AvatarTestWidget(UserData* userData=new UserData(), QWidget* parent=NULL);
		void setUserData(UserData* userData);
	protected slots:
		void speak();
	protected:
		void initialize();
		QGridLayout mGMainLayout;

		QGraphicsView mGraphicsView;
		QGraphicsScene mGraphicsScene;
		QLineEdit mTextBox;
		QPushButton mDoneButton;
		
		UserData* mUserData;
		GraphicsAvatarItem* mAvatar;
		GraphicsSpeechBalloonItem* mSpeechBalloon;
};

class AvatarDesignAndTestWidget: public QWidget {
	Q_OBJECT
	public:
		AvatarDesignAndTestWidget(UserData* userData= new UserData(), QWidget* parent=NULL);
		AvatarDesignWidget* designWidget();
		AvatarTestWidget* testWidget();
		
	protected slots:
		void switchEm();
	protected:
		void initialize();	
		QGridLayout* mGMainLayout;
		SwitchWidget mSwitch;
		AvatarDesignWidget mDesign;
		AvatarTestWidget mTest;
		QPushButton mSwitchButton;
		UserData* mUserData;
		bool mIsDesign;
};

#endif
