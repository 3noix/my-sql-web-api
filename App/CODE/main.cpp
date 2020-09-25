#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"
#include "SqlConnection.h"


int main(int argc, char *argv[])
{
	QApplication app{argc,argv};

	// connecting to database
	SqlConnection connection;
	if (!connection.connectToDatabase())
	{
		QMessageBox::critical(nullptr,"Error","Failed to connect to database");
		return 1;
	}

	// create and display the main window
	MainWindow w;
	w.show();
	return app.exec();
}

