#ifndef ENTRY
#define ENTRY


#include <QString>
#include <QDateTime>
#include <QMetaType>


struct Entry
{
	int id;
	QString description;
	int number;
	QDateTime last_modif;
};

enum class ChangeType
{
	Addition,
	Modification,
	Deletion
};

struct EntryChange
{
	ChangeType changeType;
	Entry entry;
};


Q_DECLARE_METATYPE(Entry)


#endif

