#include <QCoreApplication>
#include <iostream>
#include <QHttpServer>


int main(int argc, char *argv[])
{
	QCoreApplication app{argc,argv};
	QStringList args = app.arguments();
	args.removeFirst();
	
	if (args.size() != 1)
	{
		std::cout << "One argument expected" << std::endl;
		std::cout << "Press a key to quit" << std::endl;
		std::cin.get();
		return 1;
	}
	
	const QString &resourcesDir = args[0];
	QHttpServer httpServer;
	
	httpServer.route("/", [resourcesDir] () {
		return QHttpServerResponse::fromFile(resourcesDir + "/index.html");
	});
	httpServer.route("/<arg>", [resourcesDir] (const QString &fileName) {
		return QHttpServerResponse::fromFile(resourcesDir + "/" + fileName);
	});
	
	httpServer.listen(QHostAddress::Any,8080);
	std::cout << "HTTP server is on" << std::endl;
	std::cout << "Press Ctrl+C to terminate" << std::endl;
	return app.exec();
}

