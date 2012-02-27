#include "BrowsingWidget.h"

CustomGraphicsView::CustomGraphicsView(QWebView* webView) {
	//Does not have ownership
	mWebView = webView;
	mWebView->installEventFilter(this);

	setFrameShape(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setAlignment(Qt::AlignLeft|Qt::AlignTop);
	setResizeAnchor(QGraphicsView::NoAnchor);
	setTransformationAnchor(QGraphicsView::NoAnchor);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);

	mHideShowTimer.setDuration(CGV_HIDESHOW_DURATION);
	mHideShowTimer.setUpdateInterval(0);
	connect(&mHideShowTimer, SIGNAL(valueChanged(qreal)), this, SLOT(visibilityChanging(qreal)));
	connect(&mHideShowTimer, SIGNAL(finished()), this, SLOT(toggleVisible()));

	mIsHidden = false;

	//setViewport(new QGLWidget(QGLFormat(QGL::AlphaChannel)));
	//setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	
	setOptimizationFlags(QGraphicsView::DontSavePainterState|QGraphicsView::DontAdjustForAntialiasing);

}

CustomGraphicsView::~CustomGraphicsView() {
}

bool CustomGraphicsView::isHidden() {
	return mIsHidden;
}

void CustomGraphicsView::toggleVisible() {
	//resetTransform();
	if(mHideShowTimer.direction() == QTimeLine::Forward){
	       	//setVisible(true);
		GraphicsAvatarItem::setGlobalScale(1);
		GraphicsAvatarItem::setGlobalOpacity(1);
		mIsHidden = false;
		emit shown();
	} else {
		//setVisible(false);
		GraphicsAvatarItem::setGlobalScale(0);
		GraphicsAvatarItem::setGlobalOpacity(0);
		mIsHidden = true;
		emit hidden();
	}
	/*for(int i=0; i<items().count(); i++) {
		if(items()[i]->parentItem() == NULL) {
			items()[i]->resetMatrix();
			items()[i]->scale(GraphicsAvatarItem::globalScale(), GraphicsAvatarItem::globalScale());
		}
	}*/
}	

void CustomGraphicsView::animatedShow() {
	setVisible(true);
	mHideShowTimer.stop();
	mHideShowTimer.setDirection(QTimeLine::Forward);
	mHideShowTimer.setCurrentTime(0);
	mHideShowTimer.start();
}	
void CustomGraphicsView::animatedHide() {
	setVisible(true);
	mHideShowTimer.stop();
	mHideShowTimer.setDirection(QTimeLine::Backward);
	mHideShowTimer.setCurrentTime(mHideShowTimer.duration());
	mHideShowTimer.start();
}
void CustomGraphicsView::visibilityChanging(qreal value) {
	GraphicsAvatarItem::setGlobalScale(2-value);
	GraphicsAvatarItem::setGlobalOpacity(value);
	update();
	//QTransform t = transform();
	//t.scale(1.01, 1.01);
	//setTransform(t);
//	QTransform t;
//	t.scale(2-value, 2-value);
//	setTransform(t);
 	/*for(int i=0; i<items().count(); i++) {
		if(items()[i]->parentItem() == NULL) {
			items()[i]->resetMatrix();
			items()[i]->scale(2-value, 2-value); //GraphicsAvatarItem::globalScale(), GraphicsAvatarItem::globalScale());
		} 
	} */  
	//update();

}

void CustomGraphicsView::mousePressEvent(QMouseEvent* evt) {
	QMouseEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::mousePressEvent(evt);
}

void CustomGraphicsView::mouseReleaseEvent(QMouseEvent* evt) {
	QMouseEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::mouseReleaseEvent(evt);
	emit mouseClicked(evt->x(), evt->y());
}

void CustomGraphicsView::mouseMoveEvent(QMouseEvent* evt) {
	QMouseEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
	setCursor(mWebView->cursor());
	QGraphicsView::mouseMoveEvent(evt);
}

void CustomGraphicsView::enterEvent(QEvent* evt) {
	QEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::enterEvent(evt);
}
void CustomGraphicsView::leaveEvent(QEvent* evt) {
	QEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::leaveEvent(evt);
}
void CustomGraphicsView::focusInEvent(QFocusEvent* evt) {
	QFocusEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::focusInEvent(evt);
}
void CustomGraphicsView::focusOutEvent(QFocusEvent* evt) {
	QFocusEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::focusOutEvent(evt);
}

void CustomGraphicsView::keyPressEvent(QKeyEvent* evt) {
	QKeyEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
	//QGraphicsView::keyPressEvent(evt);
}
void CustomGraphicsView::keyReleaseEvent(QKeyEvent* evt) {
	QKeyEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
//	QGraphicsView::keyReleaseEvent(evt);
}
void CustomGraphicsView::changeEvent(QEvent* evt) {
	QEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
        QGraphicsView::changeEvent(evt);
}	

void CustomGraphicsView::wheelEvent(QWheelEvent* evt) {
	QWheelEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
//	QGraphicsView::changeEvent(evt);
} 
void CustomGraphicsView::showEvent(QShowEvent* evt) {
	QShowEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
}

void CustomGraphicsView::actionEvent(QActionEvent* evt) {
	QActionEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
}

void CustomGraphicsView::contextMenuEvent(QContextMenuEvent* evt) {
	QContextMenuEvent newEvent = *evt;
	QApplication::sendEvent(mWebView, &newEvent);
	QGraphicsView::contextMenuEvent(evt);
}	

bool CustomGraphicsView::eventFilter(QObject* obj, QEvent*) {
	if(obj != mWebView) return false;
	return false;
}

BrowsingWidget::BrowsingWidget() {
	mIsPageResolved = false;	
	mLoadProgress = 0;
	setAttribute(Qt::WA_DeleteOnClose);

	mCommunicator = new JavascriptCommunicator();

	mWebView = new QWebView();
	//mWebView->page()->settings()->setMaximumPagesInCache(0);
	//mWebView->page()->settings()->setObjectCacheCapacities(0,0,0);
	mWebView->setContextMenuPolicy(Qt::CustomContextMenu);
	//mWebView->page()->setForwardUnsupportedContent(true);
	mWebView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	connect(mWebView->page(), SIGNAL(linkClicked(const QUrl&)), Controller::self(), SIGNAL(linkActivated(const QUrl&)));

	//mWebView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);


	mGraphicsView = new CustomGraphicsView(mWebView);
	mGraphicsScene = new QGraphicsScene();	
	//mGraphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);
	mGraphicsView->setScene(mGraphicsScene);


	QPalette p(mGraphicsView->palette());
	p.setBrush(QPalette::Window, Qt::NoBrush);
	mGraphicsView->setPalette(p);
	mGraphicsView->setBackgroundRole(QPalette::Window);
		
	mGridLayout = new QGridLayout();
	mGridLayout->addWidget(mWebView, 0,0);
	mGridLayout->addWidget(mGraphicsView, 0,0);
	mGridLayout->setContentsMargins(0,0,0,0);

	setLayout(mGridLayout);		
	//setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequested(const QPoint&)));
	connect(mGraphicsView, SIGNAL(shown()), this, SIGNAL(avatarsShown()));
	connect(mGraphicsView, SIGNAL(hidden()), this, SIGNAL(avatarsHidden()));
	connect(mWebView, SIGNAL(loadFinished(bool)), this, SLOT(catchLoadFinished(bool)));
	connect(mWebView, SIGNAL(loadProgress(int)), this, SLOT(catchLoadProgress(int)));
	connect(mWebView, SIGNAL(loadStarted()), this, SLOT(catchLoadStarted()));
	connect(mGraphicsView, SIGNAL(mouseClicked(int, int)), this, SIGNAL(mouseClicked(int, int)));
	connect(mWebView, SIGNAL(urlChanged(const QUrl&)), this, SLOT(urlChanged(const QUrl&)));
	connect(mWebView, SIGNAL(titleChanged(const QString&)), this, SLOT(catchTitleChanged(const QString&)));
	connect(mWebView, SIGNAL(iconChanged()), this, SLOT(catchIconChanged()));


	connect(mWebView->page(), SIGNAL(downloadRequested(const QNetworkRequest&)), this, SLOT(catchDownloadRequested(const QNetworkRequest&))); 
	connect(mWebView->page(), SIGNAL(unsupportedContent(QNetworkReply*)), this, SLOT(unsupportedContent(QNetworkReply*)));	
	connect(mWebView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(prepareJavascript()));

	connect(mCommunicator, SIGNAL(domClicked(int, int, unsigned int)), this, SLOT(jsDomClicked(int, int, unsigned int)));

	connect(this, SIGNAL(logHistory(const QString&, const QUrl&)), Controller::self(), SIGNAL(logHistory(const QString&, const QUrl&)));

}

BrowsingWidget::~BrowsingWidget() {
       if(mGraphicsView) {
	       delete mGraphicsView;
	       mGraphicsView = NULL;
       }
       if(mWebView) { 
	       delete mWebView;
	       mWebView = NULL;
       }
       // Free ME!
       if(mCommunicator) {
		delete mCommunicator;
		mCommunicator = NULL;
       }
}

QWebView* BrowsingWidget::webView() {
	return mWebView;
}	

QGraphicsView* BrowsingWidget::graphicsView() {
	return mGraphicsView;
}
QGraphicsScene* BrowsingWidget::graphicsScene() {
	return mGraphicsScene;
}

void BrowsingWidget::pageGeometryChange(const QRect&) {}
void BrowsingWidget::scrolled(int, int) {
	mGraphicsView->resetTransform();
	mGraphicsView->translate(-mWebView->page()->currentFrame()->scrollBarValue(Qt::Horizontal), -mWebView->page()->currentFrame()->scrollBarValue(Qt::Vertical)); 
}
void BrowsingWidget::linkHovering() {}

void BrowsingWidget::resetSceneRect() {
	if(mWebView && mWebView->page() && mWebView->page()->mainFrame()) {
		QRect sceneRect(0,0,mWebView->page()->mainFrame()->contentsSize().width(), mWebView->page()->mainFrame()->contentsSize().height());
		mGraphicsScene->setSceneRect(sceneRect);
		mGraphicsView->setSceneRect(sceneRect);
	}
}

void BrowsingWidget::resizeEvent(QResizeEvent*) {
	if(mWebView && mGraphicsScene && mGraphicsView) resetSceneRect();
}

//Sets the graphics view to be visible;
void BrowsingWidget::catchLoadFinished(bool okay) {
	//if(okay == false) mIsPageResolved = false;

	//mLoadProgress = 100;
	//Scroll Requested now handled in Javascript
	//QObject::connect(mWebView->page(), SIGNAL(scrollRequested(int, int, const QRect&)), this, SLOT(scrolled(int, int)));
	QObject::connect(mCommunicator, SIGNAL(domScrolled(int, int)), this, SLOT(scrolled(int, int)));
	QObject::connect(mWebView->page(), SIGNAL(geometryChangeRequested(const QRect&)), this, SLOT(pageGeometryChange(const QRect&)));
	QObject::connect(mWebView->page(), SIGNAL(linkHovered(const QString&, const QString&, const QString&)), this, SLOT(linkHovering()));

	resetSceneRect();

	//Re-read on every reload of the page for now.
	//Helps with debugging.
	if(QFile::exists(":/js/javascript.js")) {
		QString fileText;
		QFile file(":/js/javascript.js");
		file.open(QFile::ReadOnly);
		QTextStream stream(&file);
		fileText = stream.readAll();
		mWebView->page()->mainFrame()->evaluateJavaScript(fileText) ;
	}
	if(QFile::exists(":/js/searchForNode.js")) {
		QFile file(":/js/searchForNode.js");
		file.open(QFile::ReadOnly);
		QTextStream stream(&file);
		searchDOMCode = stream.readAll();
	}
	

	scrolled(0,0);
	emit loadFinished(okay, this);

//	if(mIsPageResolved)
//		emit logHistory(mWebView->title(), mWebView->url());
}
void BrowsingWidget::catchLoadStarted() {
	mIsPageResolved = false;
	mLoadProgress = 0;
	emit loadStarted(this);
}
void BrowsingWidget::catchLoadProgress(int progress) {
	mLoadProgress = progress;
	if(mIsPageResolved == false) {
		if(progress > 10 && progress < 90) {
			mCurrentResolvedUrl = mWebView->url();
			mIsPageResolved = true;
			emit pageResolved(mCurrentResolvedUrl);		
		}
	}
	emit loadProgress(progress, this);
}

void BrowsingWidget::catchTitleChanged(const QString& title) {
	emit titleChanged(title, this); 
}
void BrowsingWidget::catchIconChanged() {
	emit iconChanged(this);
}

void BrowsingWidget::urlChanged(const QUrl&) {
	mIsPageResolved = false;	
}
bool BrowsingWidget::isPageResolved() {
	return mIsPageResolved;
}
bool BrowsingWidget::avatarsVisible() {
	return !mGraphicsView->isHidden();
}
QUrl BrowsingWidget::resolvedUrl() {
	return mCurrentResolvedUrl;
}
int BrowsingWidget::loadProgress() {
	return mLoadProgress;
}
void BrowsingWidget::setAvatarsHidden(bool hidden) {
	setAvatarsVisible(!hidden);
}
void BrowsingWidget::setAvatarsVisible(bool visible) {
	if(visible) {
		showAvatars();
	} else {
		hideAvatars();
	}
}
void BrowsingWidget::hideAvatars() {
	mGraphicsView->animatedHide();
}
void BrowsingWidget::showAvatars() {
	mGraphicsView->animatedShow();
}
void BrowsingWidget::toggleAvatars() {
	if(mGraphicsView->isHidden()) {
		mGraphicsView->animatedShow();
	} else {
		mGraphicsView->animatedHide();
	}
}

void BrowsingWidget::catchDownloadRequested(const QNetworkRequest& request) {
	download(request.url());		
}

void BrowsingWidget::unsupportedContent(QNetworkReply* reply) {
	download(reply->url()); 
}
void BrowsingWidget::download(const QUrl& url) {
	QNetworkRequest request;
	request.setUrl(url);
	emit downloadRequested(request);
}

void BrowsingWidget::prepareJavascript() {
	mWebView->page()->mainFrame()->addToJavaScriptWindowObject("communicator",mCommunicator);
}

NodeInfo BrowsingWidget::jsDomNode(unsigned int id) {
	mCommunicator->setIdToFind(id);
	QVariant v = (mWebView->page()->mainFrame()->evaluateJavaScript(searchDOMCode));
	return NodeInfo::parse(v.toString());
}	

void BrowsingWidget::jsDomClicked(int x, int y, unsigned int node) {
	emit nodeClicked(x,y, jsDomNode(node));
}

void BrowsingWidget::contextMenuRequested(const QPoint&) {}

NodeInfo::NodeInfo() {
	mId = -1;
	mX = mY = mWidth = mHeight = 0;
}

NodeInfo NodeInfo::parse(const QString& value) {
	QStringList tokens = value.split(",");
	NodeInfo n;
	if(tokens.count() >= 5) {
		n.mId = tokens[0].toUInt();
		n.mX = tokens[1].toInt();
		n.mY = tokens[2].toInt();
		n.mWidth = tokens[3].toInt();
		n.mHeight = tokens[4].toInt();
	} //else the node could not be resolved and we return the blank n
	return n;
}

unsigned int JavascriptCommunicator::hash(const QString& string) {
	return qHash(string);
}
void JavascriptCommunicator::clicked(int x, int y, unsigned int node) {
	emit domClicked(x,y,node);
}
void JavascriptCommunicator::scrolled(int x, int y) {
	emit domScrolled(x,y); 
}

NodeInfo JavascriptCommunicator::generateNode(unsigned int, int x, int y, int width, int height) {
	NodeInfo n;
	n.mX = x;
	n.mY = y;
	n.mWidth = width;
	n.mHeight = height;
	return n;
}

