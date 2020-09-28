#include "SqlTransaction.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>

namespace SqlTransaction
{

///////////////////////////////////////////////////////////////////////////////
// RESUME :
//
//  START TRANSACTION
//  ROLLBACK TRANSACTION
//  COMMIT TRANSACTION
///////////////////////////////////////////////////////////////////////////////

// remark1: because the MySQL plugin of Qt 5.11 says that it does not support transaction...
//          .. despite it does, of course!
// remark2: Qt does not support MySQL after 5.11 (the open source version at least)

// START TRANSACTION //////////////////////////////////////////////////////////
bool startTransaction()
{
	bool bSupportTransactions = QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions);
	if (bSupportTransactions) {return QSqlDatabase::database().transaction();}
	else {return QSqlQuery{}.exec("START TRANSACTION;");}
}

// ROLLBACK TRANSACTION ///////////////////////////////////////////////////////
bool rollbackTransaction()
{
	bool bSupportTransactions = QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions);
	if (bSupportTransactions) {return QSqlDatabase::database().rollback();}
	else {return QSqlQuery{}.exec("ROLLBACK;");}
}

// COMMIT TRANSACTION /////////////////////////////////////////////////////////
bool commitTransaction()
{
	bool bSupportTransactions = QSqlDatabase::database().driver()->hasFeature(QSqlDriver::Transactions);
	if (bSupportTransactions) {return QSqlDatabase::database().commit();}
	else {return QSqlQuery{}.exec("COMMIT;");}
}

}

