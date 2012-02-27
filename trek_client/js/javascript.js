String.prototype.trim = function() {
		return this.replace(/^\s+|\s+$/g,"");
}

function generatePlutoID(node) {
	var s,i, trimmedNode, collectedHash, hash;
	//alert(node.nodeType + " " + node.tagName + " " +node.nodeValue);
	if(node.nodeType == 1) {
		//alert("Element Node");
		//Loop through the children
		//create this nodes string
		s = node.tagName + "_";
		collectedHash = "";

		for(i=0; i<node.childNodes.length; i++) {
			hash = generatePlutoID(node.childNodes[i]);
			if(hash && hash.length > 0) {//hash !== undefined) {
				collectedHash += hash;
			}
		}
		//if(node.tagName == "P") alert("_"+collectedHash);
		s+=collectedHash;
		//if(node.getAttribute("id") == null) {	
			if(node.tagName.toUpperCase() == "BODY") {
				node.setAttribute("plutoid", 1234);
			} else {
				node.setAttribute("plutoid", communicator.hash(s));
			}
		//} else {
		//	node.setAttribute("plutoid", node.getAttribute("id"));
		//}
	       	return collectedHash;
	} else if(node.nodeType == 3) {
		trimmedNode = node.nodeValue.trim();
		if(trimmedNode.length > 0) {
			return trimmedNode;
		} 
		return null;
		//alert("returning (3):N/A");
	}
	return "End";
}

/*function generatePlutoID(node) {
	var s,child,i;
	if(node.nodeType == 1) {
		s = node.parentNode.tagName +  node.tagName;
		for(i=0; i<node.childNodes.length; i++) {
			child = node.childNodes[i];
			if(child.nodeType == 1) {
				s += child.tagName;
			} else if(child.nodeType == 3) {
				s += child.nodeValue.trim();
			}		
		}
	}
	return s;
}*/

function plutoIDElements(node) {
	alert("ID for Node: " + node.tagName + " is: " + generatePlutoID(node)); 	
}

document.body.onclick = function(event) { 
	var plutoid;
	plutoid = -1;
	if(event.target.hasAttribute("plutoid")) {
		plutoid = event.target.getAttribute("plutoid");
		//alert(event.target.childNodes.length);
	//	if(event.target.childNodes.length > 13) {
			//plutoid = 1234;
	//	}
	} else {
		return;
	}

	communicator.clicked(event.x, event.y, plutoid);
	
//	if(event.target.hasAttribute("plutoid")) {
//		id = event.target.getAttribute("plutoid");
		//alert(id);
//	}
}

document.onscroll = function(event) {
	communicator.scrolled(0,0);
//	alert("Scrolled");
}

generatePlutoID(document.body);
