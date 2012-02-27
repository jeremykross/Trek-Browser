#ifndef DOMLOCATION_H
#define DOMLOCATION_H

#include <QObject>
#include <QString>
#include <QList>

class DOMNode {
	public:
		QString mNodeName;
		QString mNodeValue;
};
class DOMLocation:public QObject {
	Q_OBJECT
	public:
		DOMLocation();
		void addNode(const DOMNode& node);
		void addNode(const QString& nodeName, const QString& nodeValue);
		DOMNode nextNode();
		DOMNode prevNode();
		QList<DOMNode> mNodes;
		int mCurrentNode;

};


#endif
