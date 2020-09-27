#ifndef ENTRY
#define ENTRY


#include <QString>
#include <QDateTime>


struct Entry
{
	int id = 0;
	QString description = "";
	int number = 0;
	QDateTime last_modif = {};
};


#endif

