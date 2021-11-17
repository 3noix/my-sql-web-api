#ifndef ENTRY_CHANGE
#define ENTRY_CHANGE


#include "../../web-api/src/Entry.h"


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


#endif

