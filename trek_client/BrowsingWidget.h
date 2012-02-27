#ifndef BROWSINGWIDGET_H
#define BROWSINGWIDGET_H

/* Core */
#include <QApplication>
#include <QWidget>
#include <QGLWidget>
#include <QGLFormat>
#include <QFrame>
#include <QObject>
#include <QThread>
#include <QMessageBox>
#include <QList>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QVariant>
#include <QTransform>

/* Webkit */
#include <QWebView>
#include <QWebView>
#include <QWebFrame>
#include <QWebSettings>
#include <QUrl>

/* Graphics */
#include <QGraphicsView>
#include <QGraphicsScene>

/* Appearance */
#include <QGridLayout>
#include <QPixmap>
#include <QPalette>
#include <QRect>
#include <QRectF>
#include <QPoint>

/* Events */
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QShowEvent>
#include <QActionEvent>
#include <QContextMenuEvent>
#include <QResizeEvent>

#include <QCursor>

/* IO */
#include <QFile>
#include <QTextStream>
#include <QNetworkRequest>
#include <QNetworkReply>

/* Timing */
#include <QTimeLine>

/* My Stuff */
#include "Controller.h"
#include "GraphicsAvatarItem.h"
#include "GraphicsSpeechBalloonItem.h"
#include "javascript_source.h"
#include "DownloadManager.h"


/* Defines */
#define CGV_HIDESHOW_DURATION 300 


/* CustomGraphicsView */
//This class intercepts events given to the graphics view and passes them on to the QWebView on which it will be overlayed.
class CustomGraphicsView : public QGraphicsView {
	Q_OBJECT
	public:
                CustomGraphicsView(QWebView* webView);
                ~CustomGraphicsView();

		bool eventFilter(QObject* obj, QEvent* evt);
		void animatedShow();
		void animatedHide();
		bool isHidden();
	signals:
		void mouseClicked(int x, int y);
		void shown();
		void hidden();
	protected slots:
		void visibilityChanging(qreal value);
		void toggleVisible();
	protected:
		/* Events */
	//	bool event(QEvent* evt);
		void mousePressEvent(QMouseEvent* evt);
		void mouseReleaseEvent(QMouseEvent* evt);
		void mouseMoveEvent(QMouseEvent* evt);
		void keyPressEvent(QKeyEvent* evt);
		void keyReleaseEvent(QKeyEvent* evt);
		void enterEvent(QEvent* evt);
		void leaveEvent(QEvent* evt);
		void focusInEvent(QFocusEvent* evt);
		void focusOutEvent(QFocusEvent* evt);
		void changeEvent(QEvent* evt);
		void wheelEvent(QWheelEvent* evt);
		void showEvent(QShowEvent* evt);
		void actionEvent(QActionEvent* evt);
		void contextMenuEvent(QContextMenuEvent* evt);

		QWebView* mWebView;

		QTimeLine mHideShowTimer;
		bool mIsHidden;
};

class NodeInfo {
	public:
		NodeInfo();
		static NodeInfo parse(const QString& value);
		int mId;
		int mX,mY;
		int mWidth, mHeight;
};

class JavascriptCommunicator:public QObject {
	Q_OBJECT
	Q_PROPERTY(unsigned int mIdToFind READ idToFind() WRITE setIdToFind());
	public:
		JavascriptCommunicator() {mIdToFind = 0;}
		unsigned int idToFind() { return mIdToFind; }
		void setIdToFind(unsigned int id) { mIdToFind = id; }
	signals: 
		void domClicked(int x, int y, unsigned int node);
		void domScrolled(int x, int y);

	public slots:
		NodeInfo generateNode(unsigned int id, int x, int y, int width, int height);
		void clicked(int x, int y, unsigned int node); 
		void scrolled(int x, int y);
		unsigned int hash(const QString& string);
	protected:
		unsigned int mIdToFind;
};

class BrowsingWidget : public QWidget {
	Q_OBJECT
	public:
		BrowsingWidget();
                ~BrowsingWidget();
		QWebView* webView();
		QGraphicsView* graphicsView();
		QGraphicsScene* graphicsScene();
		bool isPageResolved();
		bool avatarsVisible();
		QUrl resolvedUrl();
		void download(const QUrl& url);
		int loadProgress();
	public slots: 
		NodeInfo jsDomNode(unsigned int id);
		void scrolled(int dx, int dy);
		void resetSceneRect();
		void setAvatarsHidden(bool hidden);
		void setAvatarsVisible(bool visible);
		void hideAvatars();
		void showAvatars();
		void toggleAvatars();
	signals:
		void loadFinished(bool okay, BrowsingWidget* me);
		void loadStarted(BrowsingWidget* me);
		void loadProgress(int progress, BrowsingWidget* me);
		void titleChanged(const QString& title, BrowsingWidget* me);
		void pageResolved(const QUrl& movingTo);
		void mouseClicked(int x, int y);
		void nodeClicked(int x, int y, NodeInfo info);
		void downloadRequested(const QNetworkRequest& request);
		void iconChanged(BrowsingWidget* me);
		void logHistory(const QString& name, const QUrl& url);

		void avatarContextMenuRequested(UserData* data);

		void avatarsShown();
		void avatarsHidden();
	protected slots:
		void pageGeometryChange(const QRect& geom);
		void linkHovering();
		void catchLoadFinished(bool okay);
		void catchLoadProgress(int progress);
		void catchLoadStarted();
		void catchTitleChanged(const QString& title);
		void catchIconChanged();
		void catchDownloadRequested(const QNetworkRequest& request);
		void urlChanged(const QUrl& url);
		void unsupportedContent(QNetworkReply* reply);
		void prepareJavascript();
		void jsDomClicked(int x, int y, unsigned int node); 

		void contextMenuRequested(const QPoint&);
	protected:
		void resizeEvent(QResizeEvent* evt);
		QWebView* mWebView;
		bool mIsPageLoaded;
		bool mIsPageResolved;
		QUrl mCurrentResolvedUrl;
		CustomGraphicsView* mGraphicsView;
		QGraphicsScene* mGraphicsScene;
		QGridLayout* mGridLayout;	

		QString mapDOMCode;
		QString searchDOMCode;

		JavascriptCommunicator* mCommunicator;

		int mLoadProgress;
};

#endif
