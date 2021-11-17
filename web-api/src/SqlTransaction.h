#ifndef SQL_TRANSACTION_H
#define SQL_TRANSACTION_H


namespace SqlTransaction
{
	bool startTransaction();
	bool rollbackTransaction();
	bool commitTransaction();
}


#endif

