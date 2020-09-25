#include "MainWindow.h"
#include "EntryTableModel.h"
#include "EntryDialog.h"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QTableView>
#include <QHeaderView>
#include <QTimer>

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>


///////////////////////////////////////////////////////////////////////////////
//  CONSTRUCTEUR
//  CREATE ACTIONS
//  SETUP WIDGET
//
//  GET ENTRIES
//  SLOT REFRESH
//  SLOT ADD
//  SLOT EDIT
//  SLOT REMOVE
///////////////////////////////////////////////////////////////////////////////


// CONSTRUCTEUR ///////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) : QWidget{parent}
{
	this->createActions();
	this->setupWidget();
	this->resize(700,500);
	
	m_nbSecsRefresh = 5;
	m_refreshTimer = new QTimer{this};
	m_refreshTimer->setInterval(1000*m_nbSecsRefresh);

	QObject::connect(actionAdd,    &QAction::triggered, this, &MainWindow::slotAdd);
	QObject::connect(actionEdit,   &QAction::triggered, this, &MainWindow::slotEdit);
	QObject::connect(actionRemove, &QAction::triggered, this, &MainWindow::slotRemove);
	QObject::connect(m_refreshTimer, &QTimer::timeout,  this, &MainWindow::slotRefresh);
	
	m_lastModif = QDateTime::currentDateTime();
	m_refreshTimer->start();
	m_bFirstTime = true;
	this->slotRefresh();
}

// CREATE ACTIONS /////////////////////////////////////////////////////////////
void MainWindow::createActions()
{
	actionAdd = new QAction{"Add",this};
	actionAdd->setIcon(QIcon{":/icons/add.png"});

	actionEdit = new QAction{"Edit",this};
	actionEdit->setIcon(QIcon{":/icons/edit.png"});

	actionRemove = new QAction{"Remove",this};
	actionRemove->setIcon(QIcon{":/icons/remove.png"});
}

// SETUP WIDGET ///////////////////////////////////////////////////////////////
void MainWindow::setupWidget()
{
	layout = new QGridLayout{this};
	this->setLayout(layout);

	labelLastRefresh = new QLabel{this};

	buttonAdd = new QToolButton{this};
	buttonEdit = new QToolButton{this};
	buttonRemove = new QToolButton{this};
	
	buttonAdd->setDefaultAction(actionAdd);
	buttonEdit->setDefaultAction(actionEdit);
	buttonRemove->setDefaultAction(actionRemove);

	m_vue = new QTableView{this};
	m_modele = new EntryTableModel{this};
	m_vue->setModel(m_modele);
	m_vue->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_vue->setSelectionMode(QAbstractItemView::SingleSelection);
	m_vue->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_vue->verticalHeader()->hide();
	m_vue->setSortingEnabled(false);
	m_vue->setColumnWidth(0,80);
	m_vue->setColumnWidth(1,250);
	m_vue->setColumnWidth(2,100);
	m_vue->setColumnWidth(3,200);

	layout->addWidget(buttonAdd,0,0,1,1);
	layout->addWidget(buttonEdit,1,0,1,1);
	layout->addWidget(buttonRemove,2,0,1,1);
	layout->addWidget(labelLastRefresh,0,1,1,1);
	layout->addWidget(m_vue,1,1,3,1);
}





// GET ENTRIES ////////////////////////////////////////////////////////////////
std::vector<Entry> MainWindow::getEntries(QString *errorMessage)
{
	QString qStr = "SELECT id, description, number, last_modif FROM Entries";
	if (!m_bFirstTime) {qStr += " WHERE TIMEDIFF(last_modif,:localLastModif) >= -20";}
	qStr += ";";

	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":localLastModif",m_lastModif);

	if (!q.exec())
	{
		if (errorMessage) {*errorMessage = "getEntries failed:\n" + q.lastError().text();}
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

// SLOT REFRESH ///////////////////////////////////////////////////////////////
void MainWindow::slotRefresh()
{
	QString errorMessage;
	std::vector<Entry> entries = MainWindow::getEntries(&errorMessage);
	if (errorMessage != "")
	{
		QMessageBox::critical(this,"Refresh failed",errorMessage);
		return;
	}

	if (entries.size() > 0) {m_lastModif = m_lastRefresh;}
	for (const Entry &e : entries)
	{
		ChangeType cht = (m_bFirstTime ? ChangeType::Addition : ChangeType::Modification);
		if (!m_modele->processChange(EntryChange{cht,e},&errorMessage))
			QMessageBox::critical(this,"Error",errorMessage);
	}

	m_lastRefresh = QDateTime::currentDateTime();
	labelLastRefresh->setText(m_lastRefresh.toString("yyyy-MM-dd hh:mm:ss"));
	m_bFirstTime = false;
}

// SLOT ADD ///////////////////////////////////////////////////////////////////
void MainWindow::slotAdd()
{
	EntryDialog dialog;
	if (dialog.exec() != QDialog::Accepted) {return;}
	
	QString qStr = "INSERT INTO Entries (description, number, last_modif) VALUES (:d, :n, NOW());";
	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":d", dialog.description());
	q.bindValue(":n",dialog.number());

	if (!q.exec())
		QMessageBox::critical(this,"Addition failed",q.lastError().text());
}

// SLOT EDIT //////////////////////////////////////////////////////////////////
void MainWindow::slotEdit()
{
	QModelIndexList indexes = m_vue->selectionModel()->selectedRows(0);
	if (indexes.size() != 1) {return;}
	QModelIndex indexId = indexes[0];

	int id = m_modele->data(indexId,Qt::DisplayRole).toInt();
	QString description = m_modele->data(m_modele->index(indexId.row(),1),Qt::DisplayRole).toString();
	int number = m_modele->data(m_modele->index(indexId.row(),2),Qt::DisplayRole).toInt();

	EntryDialog dialog;
	dialog.setDescription(description);
	dialog.setNumber(number);
	if (dialog.exec() != QDialog::Accepted) {return;}

	QString qStr = "UPDATE Entries SET description=:description, number=:number WHERE id=:id;";
	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":description",dialog.description());
	q.bindValue(":number",dialog.number());
	q.bindValue(":id",id);

	if (!q.exec())
		QMessageBox::critical(this,"Modification failed",q.lastError().text());
}

// SLOT REMOVE ////////////////////////////////////////////////////////////////
void MainWindow::slotRemove()
{
	QModelIndexList indexes = m_vue->selectionModel()->selectedRows(0);
	if (indexes.size() != 1) {return;}
	QModelIndex indexId = indexes[0];
	int id = m_modele->data(indexId,Qt::DisplayRole).toInt();

	QString qStr = "DELETE FROM Entries WHERE id=:id;";
	QSqlQuery q;
	q.prepare(qStr);
	q.bindValue(":id",id);

	if (!q.exec())
		QMessageBox::critical(this,"Deletion failed",q.lastError().text());
}

