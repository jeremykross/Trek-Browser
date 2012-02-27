#include "DownloadDelegate.h"

DownloadDelegate::DownloadDelegate(QObject* parent): QStyledItemDelegate(parent) {}

void DownloadDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
//	drawFocus(painter, option, option.rect);
	if(index.column() == 1) {
		//draw the progress;
		QStyleOptionProgressBar progressOpt;
		progressOpt.rect = option.rect;
		progressOpt.minimum = 0;
		progressOpt.maximum = 100;
		int progress = (index.data().toDouble()*100);
		qlonglong kbReceived = index.data(Qt::UserRole).toLongLong()/1000;
		qlonglong kbTotal    = index.data(Qt::UserRole+1).toLongLong()/1000;
		progressOpt.progress = progress;
		if(index.data(Qt::UserRole+2).toBool()) {
			progressOpt.text = "Stopped";
		} else if(index.data(Qt::UserRole+3).toBool()) {
			progressOpt.text = "Done!";
		} else {
			progressOpt.text = QString::number(progress) + "% ( " + QString::number(kbReceived) + " kB of " + QString::number(kbTotal) + " kB )";
		}
	//	progressOpt.textVisible = true;
		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressOpt, painter);
	} else {
		QStyledItemDelegate::paint(painter, option, index);
	}
	/*painter->save();
	painter->translate(option.rect.x(), option.rect.y()+painter->fontMetrics().ascent()+1);
	
	QString downloadName = index.data(Qt::DisplayRole).toString();
	painter->drawText(0,0,downloadName);
	int nameWidth = painter->fontMetrics().width(downloadName);
	painter->drawText(nameWidth+NAME_TO_PROGRESS, 0, QString("Progress: %1 \%").arg((int)(index.data(Qt::UserRole).toDouble()*100))); 
	painter->restore();
	//drawDisplay(painter, option, option.rect, "Hello World!");
	//std::cout << "Hello paint\n";
	//painter->fillRect(option.rect, QBrush(Qt::black));	*/
}

QSize DownloadDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex&) const  {
	return QSize(-1, option.fontMetrics.height());//QItemDelegate::sizeHint(option, index);
	//return QSize(25,25);
}

