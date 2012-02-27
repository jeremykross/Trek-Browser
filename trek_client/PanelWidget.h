#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QIcon>
#include <QLabel>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QMetaType>
#include <QLineEdit>
#include <QPushButton>
class PanelWidget: public QWidget {
	Q_OBJECT
	public:
		PanelWidget(const QIcon& icon, const QString& title, QWidget* parent=NULL);
		~PanelWidget();
		void setWidget(QWidget* widget);
		QWidget* widget();
		QString title();
		QIcon icon();
		void setSearchable(bool value);
		bool searchable();
		QLineEdit* searchLine();
	signals:
		void opened();
		void closed();
	protected:
		void initialize();
		bool mSearchable;
		QWidget* mMainWidget;
		QVBoxLayout mPanelLayout;
		QHBoxLayout mSearchLayout;
		QLineEdit mSearchLine;
		QPushButton mClearButton;
		QIcon mIcon;
		QString mTitle;
		QLabel mTitleLabel;
		QLabel* mSearchLabel;
};

Q_DECLARE_METATYPE(PanelWidget*)
#endif
