#include <iostream>
#include <stack>
#include <QString>
#include <QTableWidgetItem>
#include "SceneManagerDialog.h"
#include "ui_SceneManagerDialog.h"
//----------------------------------------------------------------------------------------------------------------------
SceneManagerDialog::SceneManagerDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::SceneManagerDialog)
{
	m_ui->setupUi(this);

	//Toggle the plant visibility
	connect(m_ui->m_toggleVisibilityButton, SIGNAL(released()), this, SLOT(togglePlantVisibility()));
	// Remove plant(s)
	connect(m_ui->m_deleteButton, SIGNAL(released()), this, SLOT(removePlant()));
}
//----------------------------------------------------------------------------------------------------------------------
SceneManagerDialog::~SceneManagerDialog()
{
	delete m_ui;
}
//----------------------------------------------------------------------------------------------------------------------
void SceneManagerDialog::addRow(const QString& _plantType, const float& _x, const float& _y, const float& _z)
{
	//Insert a blank row into the table
	int row = m_ui->m_tableWidget->rowCount();
	m_ui->m_tableWidget->insertRow(row);

	//Initialise the items in the row
	QTableWidgetItem *t = new QTableWidgetItem(_plantType);
	t->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	t->setCheckState(Qt::Unchecked);
	QString position = QString::number(_x) + "," + QString::number(_y) + "," + QString::number(_z);
	QTableWidgetItem *p = new QTableWidgetItem(position);
	p->setFlags(Qt::ItemIsEnabled);
	QTableWidgetItem *b = new QTableWidgetItem("true");
	b->setFlags(Qt::ItemIsEnabled);

	//Add the items into the row
	m_ui->m_tableWidget->setItem(row,0,t);
	m_ui->m_tableWidget->setItem(row,1,p);
	m_ui->m_tableWidget->setItem(row,2,b);
}
//----------------------------------------------------------------------------------------------------------------------
void SceneManagerDialog::togglePlantVisibility()
{
	//Go through all the rows and check if the item is checked
	for (int i=0; i<m_ui->m_tableWidget->rowCount(); ++i)
	{
		if (m_ui->m_tableWidget->item(i,0)->checkState() == Qt::Checked)
		{
			//Toggle the state and emit the signal
			QString state = m_ui->m_tableWidget->item(i,2)->text();
			bool changedState = (state == "true") ? false : true;
			emit plantVisibility(i, changedState);
			QString newText = (changedState == true) ? "true" : "false";
			m_ui->m_tableWidget->item(i,2)->setText(newText);
			m_ui->m_tableWidget->item(i,0)->setCheckState(Qt::Unchecked);
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void SceneManagerDialog::removePlant()
{
	//Create a stack for rows to delete - this is so rows can be deleted in the inverse order
	std::stack<int> rowsToDelete;
	//Go through the rows and check if the item is checked
	for (int i=0; i<m_ui->m_tableWidget->rowCount(); ++i)
	{
		if (m_ui->m_tableWidget->item(i,0)->checkState() == Qt::Checked)
		{
			//Emit a signal and add the row index to the stack
			emit deletePlantSignal(static_cast<unsigned>(i));
			rowsToDelete.push(i);
		}
	}
	//Delete rows using the indices from the stack
	while (!rowsToDelete.empty())
	{
			m_ui->m_tableWidget->removeRow(rowsToDelete.top());
			rowsToDelete.pop();
	}
}
//----------------------------------------------------------------------------------------------------------------------
