function searchChildrenForId(node, id) {
	var i, testNode;
	if(node.nodeType == 1) {
		if(node.hasAttribute("plutoid")) {
			if(node.getAttribute("plutoid") == id) {
				return node;
			} else {
				//alert("Node for tag: " + node.tagName + " not equal " + node.getAttribute("plutoid"));
			}
		}
	}
	for(i=0; i<node.childNodes.length; i++) {
		testNode = searchChildrenForId(node.childNodes[i], id);
		if(testNode) return testNode;
	}
}

node = searchChildrenForId(document.body, communicator.mIdToFind);

if(!node) {
       	//alert("Error: could not identify node");
	returnVal = "-1,-1,-1,-1";
	return;
}

function getX(n) {
	var x = 0;
	while(n) {
		x+=n.offsetLeft;
		n = n.offsetParent;
	}
	return x;
}
function getY(n) {
	var y =0;
	while(n) {
		y+=n.offsetTop;
		n=n.offsetParent;
	}
	return y;
}

returnVal = communicator.mIdToFind + "," + getX(node) + "," + getY(node) + "," + node.offsetWidth + "," + node.offsetHeight;
