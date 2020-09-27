#ifndef ENTRY_CHANGE
#define ENTRY_CHANGE


#include "../../WebApi/CODE/Entry.h"


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

