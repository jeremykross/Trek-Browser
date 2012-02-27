#ifndef JAVASCRIPT_SOURCE_H
#define JAVASCRIPT_SOURCE_H

#define JS_TEST_PROGRAM "\
				function hello(event) {\
					alert(event.target.nodeName+'   '+event.target.clientWidth+'   '+event.target.clientHeight+'h');\
				}\
				document.body.onclick = hello;\
			"

#endif
