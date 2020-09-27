#include <QApplication>
#include <QMessageBox>
#include "MainWindow.h"


int main(int argc, char *argv[])
{
	QApplication app{argc,argv};
	MainWindow w;
	w.show();
	if (!w.connectToDatabaseApi())
	{
		QMessageBox::critical(&w,"Error","Failed to connect to API");
		return 1;
	}
	return app.exec();
}

