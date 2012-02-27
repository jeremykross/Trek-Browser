#include "DOMLocation.h"

DOMLocation::DOMLocation() {}

void DOMLocation::addNode(const QString& nodeName, const QString& nodeValue) {
	DOMNode n;
	n.mNodeName = nodeName;
	n.mNodeValue = nodeValue;
	addNode(n);
}	

void DOMLocation::addNode(const DOMNode& node) {
	mNodes.append(node);
}

DOMNode DOMLocation::nextNode() {
	if(mCurrentNode+1 < mNodes.length()) {
		mCurrentNode++;
		return mNodes[mCurrentNode];
	}
        return DOMNode();
}
DOMNode DOMLocation::prevNode() {
	if(mCurrentNode-1 >= 0) {
		mCurrentNode--;
		return mNodes[mCurrentNode];	
	}
        return DOMNode();
}
