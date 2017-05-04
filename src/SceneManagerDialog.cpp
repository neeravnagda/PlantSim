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

	//Set the current row
	connect(m_ui->m_tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(setCurrentRow(int,int,int,int)));
	//Toggle the plant visibility
	connect(m_ui->m_toggleVisibilityButton, SIGNAL(released()), this, SLOT(togglePlantVisibility()));

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
	t->setFlags(Qt::ItemIsEnabled);
	QTableWidgetItem *p = new QTableWidgetItem(position);
	p->setFlags(Qt::ItemIsEnabled);
	QTableWidgetItem *b = new QTableWidgetItem("true");
	b->setFlags(Qt::ItemIsEnabled);
	m_ui->m_tableWidget->setItem(row,0,t);
	m_ui->m_tableWidget->setItem(row,1,p);
	m_ui->m_tableWidget->setItem(row,2,b);
}

void SceneManagerDialog::setCurrentRow(int _r, int _c, int _pr, int _pc)
{
	m_currentRow = _r;
}

void SceneManagerDialog::togglePlantVisibility()
{
	QString q = m_ui->m_tableWidget->item(m_currentRow,2)->text();
	bool changedState = (q == "true") ? false : true;
	emit plantVisibility(m_currentRow, changedState);

	QString newText = (changedState == true) ? "true" : "false";
	m_ui->m_tableWidget->item(m_currentRow, 2)->setText(newText);
}
