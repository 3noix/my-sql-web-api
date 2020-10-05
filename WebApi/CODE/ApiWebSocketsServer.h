#ifndef API_WEBSOCKETS_SERVER
#define API_WEBSOCKETS_SERVER


#include <QObject>
#include <QJsonDocument>
#include <vector>
#include <list>
#include "Entry.h"
class QWebSocketServer;
class QWebSocket;
class QTimer;

enum class MsgType
{
	UserName,
	LockRequest,
	UnlockRequest,
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
		void slotCheckAndPing();
		void slotPong(quint64 elapsedTime, const QByteArray &payload);
		
		
	private:
		struct User
		{
			QString name;
			QWebSocket *socket;
			QDateTime firstPing;
			QDateTime lastPong;
			bool lost;
		};
		struct UserShort
		{
			QString name;
			QWebSocket *socket;
		};

		void processUserNameNotification(const QJsonObject &obj, QWebSocket *socket);
		void processLockRequest(const QJsonObject &obj, QWebSocket *socket);
		void processUnlockRequest(const QJsonObject &obj, QWebSocket *socket);
		void processAllDataRequest(QWebSocket *socket);
		void processInsertRequest(const QJsonObject &obj, QWebSocket *socket);
		void processUpdateRequest(const QJsonObject &obj, QWebSocket *socket);
		void processDeleteRequest(const QJsonObject &obj, QWebSocket *socket);

		void releaseUserLocks(QWebSocket *socket);
		bool userHasLock(int id, QWebSocket *socket);

		static MsgType getMessageType(const QJsonDocument &doc);
		static void sendErrorMessage(QWebSocket *socket, const QString &originalMsg, const QString &errorMessage);
		static QString ethernetLocalIpAddress(bool ipv6 = false);

		quint16 m_port;
		QWebSocketServer *m_server;
		std::list<User> m_clients;
		std::map<int,UserShort> m_lockers; // key=entryId
		QTimer *m_pingTimer;

		const int dtPing = 5;
		const int dtPingFirstCheck = 13;
		const int dtPingThrFailure = 11;
};


#endif

