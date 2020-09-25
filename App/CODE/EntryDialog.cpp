#include "EntryDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCloseEvent>


///////////////////////////////////////////////////////////////////////////////
// RESUME :
//
//  CONSTRUCTEUR
//  DESTRUCTEUR
//  SETUP WIDGET
//
//  DESCRIPTION
//  SET DESCRIPTION
//  NUMBER
//  SET NUMBER
//
//  SLOT BOUTON OK
//  SLOT BOUTON CANCEL
//  CLOSE EVENT
//  KEY PRESS EVENT
///////////////////////////////////////////////////////////////////////////////


// CONSTRUCTEUR ///////////////////////////////////////////////////////////////
EntryDialog::EntryDialog(QWidget *parent) : QDialog{parent}
{
	this->setWindowIcon(QIcon{":/icons/plus.png"});
	this->setWindowTitle("Entry dialog");
	this->setWindowModality(Qt::ApplicationModal);
	
	this->setupWidget();
	this->resize(400,150);
	
	lineDescription->setFocus(Qt::TabFocusReason);
	QWidget::setTabOrder(spinNumber,lineDescription);
	QWidget::setTabOrder(lineDescription,spinNumber);
	
	QObject::connect(boutonOk,     SIGNAL(clicked()), this, SLOT(slotBoutonOk()));
	QObject::connect(boutonCancel, SIGNAL(clicked()), this, SLOT(slotBoutonCancel()));
}

// SETUP WIDGET ///////////////////////////////////////////////////////////////
void EntryDialog::setupWidget()
{
	layout = new QGridLayout{this};
	this->setLayout(layout);
	
	labelDescription = new QLabel{"Description:",this};
	lineDescription = new QLineEdit{this};
	labelNumber = new QLabel{"Number:",this};
	spinNumber = new QSpinBox{this};
	
	boutonOk = new QPushButton{"Ok",this};
	boutonCancel = new QPushButton{"Cancel",this};
	
	layout->addWidget(labelDescription,0,0,1,1);
	layout->addWidget(lineDescription,0,1,1,2);
	layout->addWidget(labelNumber,1,0,1,1);
	layout->addWidget(spinNumber,1,1,1,1);
	layout->addItem(new QSpacerItem{0,10},2,0,1,3);
	layout->addWidget(boutonOk,3,0,1,2);
	layout->addWidget(boutonCancel,3,2,1,1);
}






// DESCRIPTION //////////////////////////////////////////////////////////////////////
QString EntryDialog::description() const
{
	return lineDescription->text();
}

// SET DESCRIPTION ////////////////////////////////////////////////////////////
void EntryDialog::setDescription(const QString &desc)
{
	lineDescription->setText(desc);
}

// NUMBER /////////////////////////////////////////////////////////////////////
int EntryDialog::number() const
{
	return spinNumber->value();
}

// SET NUMBER /////////////////////////////////////////////////////////////////
void EntryDialog::setNumber(int i)
{
	spinNumber->setValue(i);
}






// SLOT BOUTON OK /////////////////////////////////////////////////////////////
void EntryDialog::slotBoutonOk()
{
	if (lineDescription->text().isEmpty()) {return;}
	this->accept();
}

// SLOT BOUTON CANCEL /////////////////////////////////////////////////////////
void EntryDialog::slotBoutonCancel()
{
	this->reject();
}

// CLOSE EVENT ////////////////////////////////////////////////////////////////
void EntryDialog::closeEvent(QCloseEvent *event)
{
	Q_UNUSED(event)
	this->reject();
}

// KEY PRESS EVENT ////////////////////////////////////////////////////////////
void EntryDialog::keyPressEvent(QKeyEvent *event)
{
	int touche = event->key();
	
	if (touche == Qt::Key_Escape)
		this->slotBoutonCancel();
	else if ((touche == Qt::Key_Return) || (touche == Qt::Key_Enter))
		this->slotBoutonOk();
	else
		QWidget::keyPressEvent(event);
}

