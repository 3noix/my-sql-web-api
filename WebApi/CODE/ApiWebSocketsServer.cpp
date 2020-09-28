#include "ApiWebSocketsServer.h"
#include "SqlInterface.h"

#include <QNetworkInterface>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <iostream>

const QJsonDocument::JsonFormat format = QJsonDocument::Compact;


///////////////////////////////////////////////////////////////////////////////
//  CONSTRUCTEUR ET DESTRUCTEUR
//  ETHERNET LOCAL IP ADDRESS
//  URL
//
//  LISTEN
//  CLOSE
//  SLOT NEW CONNECTION
//  SLOT SOCKET DISCONNECTED
//
//  SLOT PROCESS MESSAGE
//  SEND ERROR MESSAGE
//  CHECK INPUT DATA
///////////////////////////////////////////////////////////////////////////////


// CONSTRUCTEUR ET DESTRUCTEUR ////////////////////////////////////////////////
ApiWebSocketsServer::ApiWebSocketsServer(quint16 port) : QObject{}
{
	m_port = port;
	m_server = new QWebSocketServer{"WebApiServer",QWebSocketServer::NonSecureMode};
	QObject::connect(m_server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

ApiWebSocketsServer::~ApiWebSocketsServer()
{
	this->close();
}

// ETHERNET LOCAL IP ADDRESS //////////////////////////////////////////////////
QString ApiWebSocketsServer::ethernetLocalIpAddress(bool ipv6)
{
	// search for the first active ethernet network interface
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	auto isActiveEthernet = [] (const QNetworkInterface &ni) {return ni.type() == QNetworkInterface::Ethernet && (ni.flags() & QNetworkInterface::IsUp);};
	auto result1 = std::find_if(interfaces.begin(),interfaces.end(),isActiveEthernet);
	if (result1 == interfaces.end()) {return QString{};}
	
	// search for the first ip address with the right protocol
	QList<QNetworkAddressEntry> addressesEntries = result1->addressEntries();
	QAbstractSocket::NetworkLayerProtocol protocolToSearch = (ipv6 ? QAbstractSocket::IPv6Protocol : QAbstractSocket::IPv4Protocol);
	auto isIpVX = [protocolToSearch] (const QNetworkAddressEntry &nae) {return nae.ip().protocol() == protocolToSearch;};
	auto result2 = std::find_if(addressesEntries.begin(),addressesEntries.end(),isIpVX);
	if (result2 == addressesEntries.end()) {return QString{};}
	return result2->ip().toString();
}

// URL ////////////////////////////////////////////////////////////////////////
QString ApiWebSocketsServer::url() const
{
	return "ws://" + ethernetLocalIpAddress() + ":" + QString::number(m_port);
}






// LISTEN /////////////////////////////////////////////////////////////////////
bool ApiWebSocketsServer::listen(QString *errorMessage)
{
	// start the server
	if (!m_server->listen(QHostAddress::Any,m_port))
	{
		if (errorMessage) {*errorMessage = "Server failed to listen: " + m_server->errorString();}
		return false;
	}
	
	if (errorMessage) {*errorMessage = "";}
	return true;
}

// CLOSE //////////////////////////////////////////////////////////////////////
void ApiWebSocketsServer::close()
{
	m_server->close();
}

// SLOT NEW CONNECTION ////////////////////////////////////////////////////////
void ApiWebSocketsServer::slotNewConnection()
{
	QWebSocket *socket = m_server->nextPendingConnection();
	QObject::connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(slotProcessMessage(QString)));
	QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(slotSocketDisconnected()));
	m_clients.push_back(User{"Anonymous",socket});

	std::cout << "one connection (now " << m_clients.size() << " clients connected)" << std::endl;
	ApiWebSocketsServer::sendAllEntries("none (on connection)",socket);
}

// SLOT SOCKET DISCONNECTED ///////////////////////////////////////////////////
void ApiWebSocketsServer::slotSocketDisconnected()
{
	if (QWebSocket *socket = qobject_cast<QWebSocket*>(sender()))
	{
		auto socketMatches = [socket] (const User &u) {return (u.socket == socket);};
		auto userIt = std::find_if(m_clients.begin(),m_clients.end(),socketMatches);
		if (userIt != m_clients.end()) {m_clients.erase(userIt);}

		socket->abort();
		socket->deleteLater();
		std::cout << "one disconnection (now " << m_clients.size() << " clients connected)" << std::endl;
	}
}






// SLOT PROCESS MESSAGE ///////////////////////////////////////////////////////
void ApiWebSocketsServer::slotProcessMessage(const QString &msg)
{
	QWebSocket *socket = qobject_cast<QWebSocket*>(sender());
	if (!socket)
	{
		std::cout << "----------- ERROR -----------" << std::endl;
		std::cout << "Failed to identify the sender" << std::endl << std::endl;
	}
	
	// checks on the message
	QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.toUtf8());
	if (!ApiWebSocketsServer::checkInputData(jsonDoc))
	{
		ApiWebSocketsServer::sendErrorMessage(socket,msg,"Invalid input data");
		std::cout << "----------- ERROR -----------" << std::endl;
		std::cout << "Invalid input data" << std::endl << std::endl;
		return;
	}

	QJsonObject object = jsonDoc.object();
	if (object.contains("userName"))
	{
		// this type of message is sent by the client just after the connection
		QString userName = object["userName"].toString();
		auto socketMatches = [socket] (const User &u) {return (u.socket == socket);};
		auto userIt = std::find_if(m_clients.begin(),m_clients.end(),socketMatches);
		if (userIt != m_clients.end()) {userIt->name = userName;}
		return;
	}

	QString rqtType = object["rqtType"].toString();
	const QJsonValue &rqtData = object["rqtData"];
	QString errorMessage;

	if (rqtType == "getData")
	{
		ApiWebSocketsServer::sendAllEntries(msg,socket);
	}
	else if (rqtType == "insert")
	{
		// get the data from the message
		QString desc = rqtData.toObject().value("description").toString();
		int number = rqtData.toObject().value("number").toDouble();

		// insert it into the database
		Entry e = SqlInterface::insertEntry(desc,number,&errorMessage);
		if (e.id == 0 || errorMessage != "")
		{
			ApiWebSocketsServer::sendErrorMessage(socket,msg,errorMessage);
			return;
		}

		// notify all the users
		QJsonObject eObj;
		eObj.insert("id",e.id);
		eObj.insert("description",e.description);
		eObj.insert("number",e.number);
		eObj.insert("last_modif",e.last_modif.toString("yyyy-MM-dd hh:mm:ss"));
		QJsonObject obj;
		obj.insert("type","insert");
		obj.insert("entry",eObj);
		QString msg2 = QJsonDocument{obj}.toJson(format);
		for (const User &u : m_clients) {u.socket->sendTextMessage(msg2);}
	}
	else if (rqtType == "update")
	{
		// get the data from the message
		int id = rqtData.toObject().value("id").toDouble();
		QString desc = rqtData.toObject().value("description").toString();
		int number = rqtData.toObject().value("number").toDouble();

		// update the entry in the database
		Entry e = SqlInterface::updateEntry(id,desc,number,&errorMessage);
		if (e.id == 0 || errorMessage != "")
		{
			ApiWebSocketsServer::sendErrorMessage(socket,msg,errorMessage);
			return;
		}

		// notify all the users
		QJsonObject eObj;
		eObj.insert("id",e.id);
		eObj.insert("description",e.description);
		eObj.insert("number",e.number);
		eObj.insert("last_modif",e.last_modif.toString("yyyy-MM-dd hh:mm:ss"));
		QJsonObject obj;
		obj.insert("type","update");
		obj.insert("entry",eObj);
		QString msg2 = QJsonDocument{obj}.toJson(format);
		for (const User &u : m_clients) {u.socket->sendTextMessage(msg2);}
	}
	else if (rqtType == "delete")
	{
		int id = rqtData.toDouble();
		if (!SqlInterface::deleteEntry(id,&errorMessage))
		{
			ApiWebSocketsServer::sendErrorMessage(socket,msg,errorMessage);
			return;
		}

		// notify all the users
		QJsonObject obj;
		obj.insert("type","delete");
		obj.insert("id",id);
		QString msg2 = QJsonDocument{obj}.toJson(format);
		for (const User &u : m_clients) {u.socket->sendTextMessage(msg2);}
	}
}

// SEND ALL ENTRIES ///////////////////////////////////////////////////////////
bool ApiWebSocketsServer::sendAllEntries(const QString &originalMsg, QWebSocket *socket)
{
	if (!socket) {return false;}
	QString errorMessage;
	std::vector<Entry> entries = SqlInterface::getEntries(&errorMessage);
	if (errorMessage != "")
	{
		ApiWebSocketsServer::sendErrorMessage(socket,originalMsg,errorMessage);
		return false;
	}

	QJsonArray array;
	for (const Entry &e : entries)
	{
		QJsonObject obj;
		obj.insert("id",e.id);
		obj.insert("description",e.description);
		obj.insert("number",e.number);
		obj.insert("last_modif",e.last_modif.toString("yyyy-MM-dd hh:mm:ss"));
		array.push_back(obj);
	}
	
	socket->sendTextMessage(QJsonDocument{array}.toJson(format));
	return true;
}



/*
the original messages should be like this:
{
	"userName": "myPseudo"
}
{
	"rqtType": "getData",
	"rqtData": null
}
{
	"rqtType": "insert",
	"rqtData": {
		"description": "blabla",
		"number": 3
	}
}
{
	"rqtType": "update",
	"rqtData": {
		"id": 1,
		"description": "blabla",
		"number": 3
	}
}
{
	"rqtType": "delete",
	"rqtData": 1
}
*/




// SEND ERROR MESSAGE /////////////////////////////////////////////////////////
void ApiWebSocketsServer::sendErrorMessage(QWebSocket *socket, const QString &originalMsg, const QString &errorMessage)
{
	if (!socket) {return;}
	QJsonObject obj;
	obj.insert("originalMsg",originalMsg);
	obj.insert("errorMsg",errorMessage);
	socket->sendTextMessage(QJsonDocument{obj}.toJson(format));
}

// CHECK INPUT DATA ///////////////////////////////////////////////////////////
bool ApiWebSocketsServer::checkInputData(const QJsonDocument &doc)
{
	if (!doc.isObject()) {return false;}

	QJsonObject object = doc.object();
	if (object.contains("userName") && object["userName"].isString()) {return true;}
	if (!object.contains("rqtType")) {return false;}
	if (!object.contains("rqtData")) {return false;}
	if (!object["rqtType"].isString()) {return false;}
	
	QString rqtType = object["rqtType"].toString();
	if (rqtType == "getData")
	{
		return true;
	}
	else if (rqtType == "insert")
	{
		const QJsonValue &v = object["rqtData"];
		if (!v.isObject()) {return false;}
		QJsonObject rdo = v.toObject();

		if (!rdo.contains("description") || !rdo.contains("number")) {return false;}
		if (!rdo["description"].isString() || !rdo["number"].isDouble()) {return false;}
		return true;
	}
	else if (rqtType == "update")
	{
		const QJsonValue &v = object["rqtData"];
		if (!v.isObject()) {return false;}
		QJsonObject rdo = v.toObject();

		if (!rdo.contains("id") || !rdo.contains("description") || !rdo.contains("number")) {return false;}
		if (!rdo["id"].isDouble() || !rdo["description"].isString() || !rdo["number"].isDouble()) {return false;}
		return true;
	}
	else if (rqtType == "delete")
	{
		return object["rqtData"].isDouble();
	}

	return false;
}

