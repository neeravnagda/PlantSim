#include <stack>
#include <QString>
#include <QTableWidgetItem>
#include "SceneManagerDialog.h"
#include "ui_SceneManagerDialog.h"
#include <iostream>

SceneManagerDialog::SceneManagerDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::SceneManagerDialog)
{
	m_ui->setupUi(this);

	//Toggle the plant visibility
	connect(m_ui->m_toggleVisibilityButton, SIGNAL(released()), this, SLOT(togglePlantVisibility()));
	// Remove plant(s)
	connect(m_ui->m_deleteButton, SIGNAL(released()), this, SLOT(removePlant()));

	//Add the test plant
	addRow("test",0,0,0);
}

SceneManagerDialog::~SceneManagerDialog()
{
	delete m_ui;
}

void SceneManagerDialog::addRow(const QString& _plantType, const float& x, const float& y, const float& z)
{
	int row = m_ui->m_tableWidget->rowCount();
	m_ui->m_tableWidget->insertRow(row);
	QString position = QString::number(x) + "," + QString::number(y) + "," + QString::number(z);

	QTableWidgetItem *t = new QTableWidgetItem(_plantType);
	t->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	t->setCheckState(Qt::Unchecked);
	QTableWidgetItem *p = new QTableWidgetItem(position);
	p->setFlags(Qt::ItemIsEnabled);
	QTableWidgetItem *b = new QTableWidgetItem("true");
	b->setFlags(Qt::ItemIsEnabled);
	m_ui->m_tableWidget->setItem(row,0,t);
	m_ui->m_tableWidget->setItem(row,1,p);
	m_ui->m_tableWidget->setItem(row,2,b);
}

void SceneManagerDialog::togglePlantVisibility()
{
	for (int i=0; i<m_ui->m_tableWidget->rowCount(); ++i)
	{
		if (m_ui->m_tableWidget->item(i,0)->checkState() == Qt::Checked)
		{
			QString state = m_ui->m_tableWidget->item(i,2)->text();
			bool changedState = (state == "true") ? false : true;
			emit plantVisibility(i, changedState);
			QString newText = (changedState == true) ? "true" : "false";
			m_ui->m_tableWidget->item(i,2)->setText(newText);
			m_ui->m_tableWidget->item(i,0)->setCheckState(Qt::Unchecked);
		}
	}
}

void SceneManagerDialog::removePlant()
{
	std::stack<int> rowsToDelete;
	for (int i=0; i<m_ui->m_tableWidget->rowCount(); ++i)
	{
		if (m_ui->m_tableWidget->item(i,0)->checkState() == Qt::Checked)
		{
			emit deletePlantSignal(static_cast<unsigned>(i));
			rowsToDelete.push(i);
		}
	}
	while (!rowsToDelete.empty())
	{
			m_ui->m_tableWidget->removeRow(rowsToDelete.top());
			rowsToDelete.pop();
	}
}
