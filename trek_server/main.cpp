#include <QCoreApplication>
#include <QObject>
#include <QHostAddress>

#include <iostream>

#include "Server.h"
#include "DataServer.h"

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);
	Server server;
	if(!server.listen(QHostAddress::Any, 4133)) {
		std::cout << "Failed to bind port!\n";
		return 1;	
	}

/*	DataServer dataServer;
	if(!dataServer.listen(QHostAddress::Any, 4131)) {
		std::cout << "Failed to bind data server port!\n";
		return 1;
	} */

//	QPushButton quitButton("Quit Pluto Server");
//	quitButton.setWindowTitle("Pluto Server");
//	QObject::connect(&quitButton, SIGNAL(clicked()), &app, SLOT(quit()));
//	quitButton.show();

	return app.exec();
}
