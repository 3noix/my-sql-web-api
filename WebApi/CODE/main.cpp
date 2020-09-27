#include <QCoreApplication>
#include <iostream>
#include "SqlConnection.h"
#include "ApiWebSocketsServer.h"


int main(int argc, char *argv[])
{
	QCoreApplication app{argc,argv};

	// connecting to database
	SqlConnection connection;
	if (!connection.connectToDatabase())
	{
		std::cout << "Failed to connect to database" << std::endl;
		std::cout << "Press a key to terminate" << std::endl;
		std::cin.get();
		return 1;
	}

	// create the server
	ApiWebSocketsServer server{1234};
	QString errorMessage;
	if (!server.listen(&errorMessage))
	{
		std::cout << "Failed to start server:" << std::endl;
		std::cout << qPrintable(errorMessage) << std::endl;
		std::cout << "Press a key to terminate" << std::endl;
		std::cin.get();
		return 1;
	}

	std::cout << "Running" << std::endl;
	return app.exec();
}

