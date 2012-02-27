#ifndef ADDRESSCOMBOBOX_H
#define ADDRESSCOMBOBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QRect>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QMessageBox>

#include <iostream>


class AddressLineEdit: public QLineEdit {
	Q_OBJECT
	public: 
		void setFavoriteIcon(const QIcon& icon);
	public slots:
		void setFavicon(const QIcon& icon);
		void clearFavicon();
	signals:
		void favoriteIconClicked();
	protected:
		AddressLineEdit(QWidget* parent=NULL);
		void paintEvent(QPaintEvent* evt);
		void mouseMoveEvent(QMouseEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);
		void leaveEvent(QEvent* evt);
		void setPercentComplete(int value);
		void setupMargins();
				
		QIcon mFavoriteIcon;
		QIcon mFavicon;

		int percentComplete;
		int mLeft, mRight;

		
		friend class AddressComboBox;
};

class AddressComboBox: public QComboBox {
	Q_OBJECT
	public:
		AddressComboBox(QWidget* parent=NULL);
		AddressLineEdit* addressLineEdit();
	public slots:
		void setPercentComplete(int value);
	protected:
		void paintEvent(QPaintEvent* evt);

		AddressLineEdit customLineEdit;
};


#endif
