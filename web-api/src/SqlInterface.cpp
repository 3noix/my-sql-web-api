#include "SqlInterface.h"
#include "SqlTransaction.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>


///////////////////////////////////////////////////////////////////////////////
// RESUME :
//
//  GET ENTRIES
//  GET ENTRY
//  INSERT ENTRY
//  UPDATE ENTRY
//  DELETE ENTRY
///////////////////////////////////////////////////////////////////////////////


namespace SqlInterface
{
// GET ENTRIES ////////////////////////////////////////////////////////////////
std::vector<Entry> getEntries(QString *errorMessage)
{
	QString qStr = "SELECT id, description, number, last_modif FROM Entries;";
	QSqlQuery q;
	if (!q.exec(qStr))
	{
		if (errorMessage) {*errorMessage = q.lastError().text();}
		return {};
	}

	std::vector<Entry> entries;
	while (q.next())
	{
		Entry e;
		e.id = q.value(0).toInt();
		e.description = q.value(1).toString();
		e.number = q.value(2).toInt();
		e.last_modif = q.value(3).toDateTime();
		entries.push_back(e);
	}

	if (errorMessage) {*errorMessage = "";}
	return entries;
}

// GET ENTRY //////////////////////////////////////////////////////////////////
Entry getEntry(int id, QString *errorMessage)
{
	QString qStr = "SELECT id, description, number, last_modif FROM Entries WHERE id=:id;";
	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":id",id);
	if (!q.exec())
	{
		if (errorMessage) {*errorMessage = q.lastError().text();}
		return {};
	}
	if (!q.next())
	{
		if (errorMessage) {*errorMessage = "No result for this id";}
		return {};
	}

	Entry e;
	e.id = q.value(0).toInt();
	e.description = q.value(1).toString();
	e.number = q.value(2).toInt();
	e.last_modif = q.value(3).toDateTime();

	if (errorMessage) {*errorMessage = "";}
	return e;
}

// INSERT ENTRY ///////////////////////////////////////////////////////////////
Entry insertEntry(const QString &description, int number, QString *errorMessage)
{
	QString qStr1 = "INSERT INTO Entries (description, number, last_modif) VALUES (:d, :n, NOW());";
	QSqlQuery q;
	q.prepare(qStr1);
	q.bindValue(":d", description);
	q.bindValue(":n",number);

	SqlTransaction::startTransaction();
	if (!q.exec())
	{
		if (errorMessage) {*errorMessage = q.lastError().text();}
		return {};
	}

	QString qStr2 = "SELECT id, last_modif FROM Entries WHERE id=LAST_INSERT_ID();";
	if (!q.exec(qStr2) || !q.next())
	{
		SqlTransaction::rollbackTransaction();
		if (errorMessage) {*errorMessage = q.lastError().text();}
		return {};
	}

	Entry e;
	e.id = q.value(0).toInt();
	e.description = description;
	e.number = number;
	e.last_modif = q.value(1).toDateTime();

	SqlTransaction::commitTransaction();
	if (errorMessage) {*errorMessage = "";}
	return e;
}

// UPDATE ENTRY ///////////////////////////////////////////////////////////////
Entry updateEntry(int id, const QString &description, int number, QString *errorMessage)
{
	QString qStr = "UPDATE Entries SET description=:d, number=:n WHERE id=:id;";
	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":d",description);
	q.bindValue(":n",number);
	q.bindValue(":id",id);

	SqlTransaction::startTransaction();
	if (!q.exec())
	{
		if (errorMessage) {*errorMessage = q.lastError().text();}
		return {};
	}

	Entry e = getEntry(id,errorMessage);
	if (e.id == 0)
	{
		SqlTransaction::rollbackTransaction();
		return {};
	}

	SqlTransaction::commitTransaction();
	if (errorMessage) {*errorMessage = "";}
	return e;
}

// DELETE ENTRY ///////////////////////////////////////////////////////////////
bool deleteEntry(int id, QString *errorMessage)
{
	QString qStr = "DELETE FROM Entries WHERE id=:id;";
	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":id",id);
	if (!q.exec())
	{
		if (errorMessage) {*errorMessage = q.lastError().text();}
		return false;
	}

	if (errorMessage) {*errorMessage = "";}
	return true;
}
}

