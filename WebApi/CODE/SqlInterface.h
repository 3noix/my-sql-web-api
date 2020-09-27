#ifndef SQL_INTERFACE
#define SQL_INTERFACE


#include <QString>
#include <vector>
#include "Entry.h"


namespace SqlInterface
{
	std::vector<Entry> getEntries(QString *errorMessage = nullptr);
	Entry getEntry(int id, QString *errorMessage = nullptr);
	Entry insertEntry(const QString &description, int number, QString *errorMessage = nullptr);
	Entry updateEntry(int id, const QString &description, int number, QString *errorMessage = nullptr);
	bool deleteEntry(int id, QString *errorMessage = nullptr);
}


#endif

