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
	InsertNotification,
	UpdateNotification,
	DeleteNotification,
	None
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
		
		
	private slots:
		void slotAdd();
		void slotEdit();
		void slotRemove();
		void slotMessageReceived(const QString &msg);
		
		
	private:
		static MsgType checkMessage(const QJsonDocument &doc);
		static bool isEntry(const QJsonObject &obj);
		static Entry toEntry(const QJsonObject &obj);

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

