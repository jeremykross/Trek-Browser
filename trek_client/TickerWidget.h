#include <QWidget>
#include <QObject>
#include <QPaintEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QPen>
#include <QSize>
#include <QFont>
#include <QTimer>
#include <QQueue>
#include <QTimeLine>
#include <QTransform>
#include <QGLWidget>
#include <QMouseEvent>
#include <QSizePolicy>

#include <QMessageBox>

#include <iostream>

class TickerWidget: public QWidget {
	Q_OBJECT
	public:
		TickerWidget(QWidget* parent=NULL);
		void initialize();
		QSize sizeHint();
		void setFont(const QFont& font);
		void addMessage(const QString& mesg);
		bool isShowingMessage();
	signals:
		void clicked(const QString& mesg);
		void done();
		void shown();
		void hidden();
	public slots:	
		void start();
		void nextMessage();
	protected slots:
		void tick();
		void nextProgress(qreal value);
	protected:
		void paintEvent(QPaintEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);
		void changeMessage();
		QFont mFont;
		int mPosX;
		QTimer mTimer;

		QString mCurrMessage;
		int mCurrWidth;
		QQueue<QString> mMessageQueue;
		float mSpeed;

		bool mSwitched;
		QTransform mTransform;
		QTimeLine mNextTimeLine;

		int mMessageDuration;
};
