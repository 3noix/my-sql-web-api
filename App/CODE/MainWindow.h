#ifndef MAIN_WINDOW
#define MAIN_WINDOW


#include <QWidget>
#include <QDateTime>
#include <QWebSocket>
#include "../../WebApi/CODE/Entry.h"
class QAction;
class QGridLayout;
class QToolButton;
class QTableView;
class EntryTableModel;

enum class MsgType
{
	AllData,
	LockAnswer,
	UnlockAnswer,
	InsertNotification,
	UpdateNotification,
	DeleteNotification,
	ErrorNotification,
	Invalid
};


class MainWindow : public QWidget
{
	Q_OBJECT
	
	public:
		MainWindow(QWidget *parent = nullptr);
		MainWindow(const MainWindow &other) = delete;
		MainWindow(MainWindow &&other) = delete;
		MainWindow& operator=(const MainWindow &other) = delete;
		MainWindow& operator=(MainWindow &&other) = delete;
		~MainWindow() = default;

		bool connectToDatabaseApi();
		bool isConnected() const;


	signals:
		void lockUnlockAnswerProcessed();
		
		
	private slots:
		void slotAdd();
		void slotEdit();
		void slotRemove();
		void slotMessageReceived(const QString &msg);
		void slotDisconnected();
		
		
	private:
		bool lockUnlockEntry(bool lock, int id, QString *errorMessage = nullptr);
		int m_lockUnlockId;
		QString m_lockUnlockStatus;
		QString m_lockUnlockErrorMsg;

		static MsgType getMessageType(const QJsonDocument &doc);

		void createActions();
		void setupWidget();
		
		QGridLayout *layout;
		QAction *actionAdd, *actionEdit, *actionRemove;
		QToolButton *buttonAdd, *buttonEdit, *buttonRemove;

		QTableView *m_vue;
		EntryTableModel *m_modele;
		QWebSocket m_webSocket;
};


#endif

