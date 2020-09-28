#include "MainWindow.h"
#include "EntryTableModel.h"
#include "EntryDialog.h"
#include "otherFunctions.h"

#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QTableView>
#include <QHeaderView>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QJsonDocument::JsonFormat format = QJsonDocument::Compact;


///////////////////////////////////////////////////////////////////////////////
//  CONSTRUCTEUR
//  CONNECT TO DATABASE API
//  SLOT DISCONNECTED
//  IS CONNECTED
//  CREATE ACTIONS
//  SETUP WIDGET
//
//  SLOT MESSAGE RECEIVED
//  GET MESSAGE TYPE
//
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
	
	QObject::connect(actionAdd,    SIGNAL(triggered()), this, SLOT(slotAdd()));
	QObject::connect(actionEdit,   SIGNAL(triggered()), this, SLOT(slotEdit()));
	QObject::connect(actionRemove, SIGNAL(triggered()), this, SLOT(slotRemove()));
	QObject::connect(&m_webSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(slotMessageReceived(QString)));
}

// CONNECT TO DATABASE API ////////////////////////////////////////////////////
bool MainWindow::connectToDatabaseApi()
{
	QEventLoop loop;
	QObject::connect(&m_webSocket, SIGNAL(connected()), &loop, SLOT(quit()));
	QObject::connect(&m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
	QObject::connect(&m_webSocket, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
	m_webSocket.open(QUrl{"ws://localhost:1234"});
	loop.exec();

	bool bConnected = (m_webSocket.isValid());
	if (bConnected)
	{
		// enable the actions
		actionAdd->setEnabled(true);
		actionEdit->setEnabled(true);
		actionRemove->setEnabled(true);

		// send the user name
		QJsonObject obj{{"userName",getWindowsUserName()}};
		m_webSocket.sendTextMessage(QJsonDocument{obj}.toJson(format));
	}

	return bConnected;
}

// SLOT DISCONNECTED //////////////////////////////////////////////////////////
void MainWindow::slotDisconnected()
{
	actionAdd->setEnabled(false);
	actionEdit->setEnabled(false);
	actionRemove->setEnabled(false);
}

// IS CONNECTED ///////////////////////////////////////////////////////////////
bool MainWindow::isConnected() const
{
	return m_webSocket.isValid();
}

// CREATE ACTIONS /////////////////////////////////////////////////////////////
void MainWindow::createActions()
{
	actionAdd = new QAction{"Add",this};
	actionAdd->setIcon(QIcon{":/icons/add.png"});
	actionAdd->setEnabled(false);

	actionEdit = new QAction{"Edit",this};
	actionEdit->setIcon(QIcon{":/icons/edit.png"});
	actionEdit->setEnabled(false);

	actionRemove = new QAction{"Remove",this};
	actionRemove->setIcon(QIcon{":/icons/remove.png"});
	actionRemove->setEnabled(false);
}

// SETUP WIDGET ///////////////////////////////////////////////////////////////
void MainWindow::setupWidget()
{
	layout = new QGridLayout{this};
	this->setLayout(layout);

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
	layout->addWidget(m_vue,0,1,4,1);
}






// SLOT MESSAGE RECEIVED //////////////////////////////////////////////////////
void MainWindow::slotMessageReceived(const QString &msg)
{
	//qDebug() << "slotMessageReceived(" << QString{msg}.remove('\"') << ")";
	QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
	MsgType msgType = MainWindow::getMessageType(doc);
	if (msgType == MsgType::Invalid) {return;}

	QString errorMessage;
	if (msgType == MsgType::AllData)
	{
		for (const QJsonValue &v : doc.array())
		{
			Entry e = Entry::fromJsonObject(v.toObject());
			if (e.id == 0) {continue;}
			if (!m_modele->processChange(EntryChange{ChangeType::Addition,e},&errorMessage))
			{
				QMessageBox::critical(this,"Error",errorMessage);
				return;
			}
		}
	}
	else if (msgType == MsgType::InsertNotification)
	{
		Entry e = Entry::fromJsonObject(doc.object()["entry"].toObject());
		if (e.id == 0) {return;}
		if (!m_modele->processChange(EntryChange{ChangeType::Addition,e},&errorMessage))
			QMessageBox::critical(this,"Error",errorMessage);
	}
	else if (msgType == MsgType::UpdateNotification)
	{
		Entry e = Entry::fromJsonObject(doc.object()["entry"].toObject());
		if (e.id == 0) {return;}
		if (!m_modele->processChange(EntryChange{ChangeType::Modification,e},&errorMessage))
			QMessageBox::critical(this,"Error",errorMessage);
	}
	else if (msgType == MsgType::DeleteNotification)
	{
		Entry e;
		e.id = doc.object()["id"].toInt();
		if (!m_modele->processChange(EntryChange{ChangeType::Deletion,e},&errorMessage))
			QMessageBox::critical(this,"Error",errorMessage);
	}
}

// GET MESSAGE TYPE ///////////////////////////////////////////////////////////
MsgType MainWindow::getMessageType(const QJsonDocument &doc)
{
	if (doc.isNull()) {return MsgType::Invalid;}
	if (doc.isArray()) {return MsgType::AllData;}
	if (!doc.isObject()) {return MsgType::Invalid;}
	
	QJsonObject obj = doc.object();
	if (obj.contains("type") && obj["type"].toString() == "insert" && obj.contains("entry")) {return MsgType::InsertNotification;}
	if (obj.contains("type") && obj["type"].toString() == "update" && obj.contains("entry")) {return MsgType::UpdateNotification;}
	if (obj.contains("type") && obj["type"].toString() == "delete" && obj.contains("id"))    {return MsgType::DeleteNotification;}
	return MsgType::Invalid;
}






// SLOT ADD ///////////////////////////////////////////////////////////////////
void MainWindow::slotAdd()
{
	EntryDialog dialog{this};
	if (dialog.exec() != QDialog::Accepted) {return;}

	QJsonObject subObj{{"description",dialog.description()},{"number",dialog.number()}};
	QJsonObject obj{{"rqtType","insert"},{"rqtData",subObj}};
	m_webSocket.sendTextMessage(QJsonDocument{obj}.toJson(format));
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

	EntryDialog dialog{this};
	dialog.setDescription(description);
	dialog.setNumber(number);
	if (dialog.exec() != QDialog::Accepted) {return;}

	QJsonObject subObj{{"id",id},{"description",dialog.description()},{"number",dialog.number()}};
	QJsonObject obj{{"rqtType","update"},{"rqtData",subObj}};
	m_webSocket.sendTextMessage(QJsonDocument{obj}.toJson(format));
}

// SLOT REMOVE ////////////////////////////////////////////////////////////////
void MainWindow::slotRemove()
{
	QModelIndexList indexes = m_vue->selectionModel()->selectedRows(0);
	if (indexes.size() != 1) {return;}
	QModelIndex indexId = indexes[0];
	int id = m_modele->data(indexId,Qt::DisplayRole).toInt();

	QJsonObject obj{{"rqtType","delete"},{"rqtData",id}};
	m_webSocket.sendTextMessage(QJsonDocument{obj}.toJson(format));
}

