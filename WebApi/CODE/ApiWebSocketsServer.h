#ifndef API_WEBSOCKETS_SERVER
#define API_WEBSOCKETS_SERVER


#include <QObject>
#include <QJsonDocument>
#include <vector>
#include <list>
#include "Entry.h"
class QWebSocketServer;
class QWebSocket;

enum class MsgType
{
	UserName,
	AllDataRequest,
	InsertRequest,
	UpdateRequest,
	DeleteRequest,
	Invalid
};


class ApiWebSocketsServer : public QObject
{
	Q_OBJECT
	
	public:
		ApiWebSocketsServer(quint16 port);
		ApiWebSocketsServer(const ApiWebSocketsServer &other) = delete;
		ApiWebSocketsServer(ApiWebSocketsServer &&other) = delete;
		ApiWebSocketsServer& operator=(const ApiWebSocketsServer &other) = delete;
		ApiWebSocketsServer& operator=(ApiWebSocketsServer &&other) = delete;
		virtual ~ApiWebSocketsServer();
		
		QString url() const;
		bool listen(QString *errorMessage = nullptr);
		void close();
		
		
	private slots:
		void slotNewConnection();
		void slotProcessMessage(const QString &msg);
		void slotSocketDisconnected();
		
		
	private:
		struct User
		{
			QString name;
			QWebSocket *socket;
		};
		
		static MsgType getMessageType(const QJsonDocument &doc);
		static bool sendAllEntries(const QString &originalMsg, QWebSocket *socket);
		static void sendErrorMessage(QWebSocket *socket, const QString &originalMsg, const QString &errorMessage);
		static QString ethernetLocalIpAddress(bool ipv6 = false);

		quint16 m_port;
		QWebSocketServer *m_server;
		std::list<User> m_clients;
};


#endif

