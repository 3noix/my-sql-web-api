#include "SqlConnection.h"

#include <QFile>
#include <QStringList>
#include <QSqlDatabase>
#include <QSqlQuery>


///////////////////////////////////////////////////////////////////////////////
// RESUME :
//
//  CONSTRUCTEUR ET DESTRUCTEUR
//
//  IS CONNECTED
//  CONNECT TO DATABASE
//  DISCONNECT FROM DATABASE
///////////////////////////////////////////////////////////////////////////////


// CONSTRUCTEUR ET DESTRUCTEUR ////////////////////////////////////////////////
SqlConnection::SqlConnection()
{
	m_bConnected = false;
}

SqlConnection::~SqlConnection()
{
	this->disconnectFromDatabase();
}




// IS CONNECTED ///////////////////////////////////////////////////////////////
bool SqlConnection::isConnected() const
{
	return m_bConnected;
}

// CONNECT TO DATABASE ////////////////////////////////////////////////////////
bool SqlConnection::connectToDatabase()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setPort(3306);
	
	db.setUserName("root");
	db.setHostName("localhost");
	db.setDatabaseName("test_db_web_api");
	db.setPassword("password");
	
	m_bConnected = db.open();
	return m_bConnected;
}

// DISCONNECT FROM DATABASE ///////////////////////////////////////////////////
void SqlConnection::disconnectFromDatabase()
{
	if (!m_bConnected) {return;}
	QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
	m_bConnected = false;
}

