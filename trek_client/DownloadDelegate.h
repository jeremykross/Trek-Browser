#ifndef DOWNLOADDELEGATE_H
#define DOWNLOADDELEGATE_H

#include <QApplication>
#include <QObject>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyle>
#include <QModelIndex>
#include <QSize>
#include <QBrush>
#include <QStyleOptionProgressBar>

#include <iostream>

#define NAME_TO_PROGRESS 3

class DownloadDelegate: public QStyledItemDelegate {
	Q_OBJECT
	public:
		DownloadDelegate(QObject* parent=NULL);
		void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const; 
		QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const; 
};


#endif
