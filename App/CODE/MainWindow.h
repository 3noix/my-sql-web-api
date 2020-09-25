#ifndef MAIN_WINDOW
#define MAIN_WINDOW


#include <QWidget>
#include <QDateTime>
#include "Entry.h"
class QAction;
class QGridLayout;
class QLabel;
class QToolButton;
class QTableView;
class QTimer;
class EntryTableModel;


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
		
		
	private slots:
		void slotAdd();
		void slotEdit();
		void slotRemove();
		void slotRefresh();
		
		
	private:
		std::vector<Entry> getEntries(QString *errorMessage = nullptr);
		void createActions();
		void setupWidget();
		
		QGridLayout *layout;
		QLabel *labelLastRefresh;
		QAction *actionAdd, *actionEdit, *actionRemove;
		QToolButton *buttonAdd, *buttonEdit, *buttonRemove;

		QTableView *m_vue;
		EntryTableModel *m_modele;
		QDateTime m_lastModif, m_lastRefresh;
		QTimer *m_refreshTimer;
		int m_nbSecsRefresh;
		bool m_bFirstTime;
};


#endif

