#include <iostream>
#include <string>
#include <unordered_set>
#include <QString>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_PlantBlueprintDialog.h"
#include "PlantBlueprint.h"
//----------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,3,1);

	m_plantBlueprintDialog = new PlantBlueprintDialog(this);

	connect(m_ui->m_updateButton, SIGNAL(released()), this, SLOT(updatePlants()));
	connect(m_ui->m_newPlantButton, SIGNAL(released()), this, SLOT(createNewPlant()));
	connect(m_ui->m_plantType, SIGNAL(currentIndexChanged(int)), this, SLOT(openPlantBlueprintDialogFromUI()));
	connect(m_ui->s_newPlantBlueprint, SIGNAL(triggered(bool)), this, SLOT(openPlantBlueprintDialogFromMenubar()));
	connect(m_plantBlueprintDialog->getUI().m_cancel, SIGNAL(released()), this, SLOT(closePlantBlueprintDialog()));
	connect(m_plantBlueprintDialog->getUI().m_create, SIGNAL(released()), this, SLOT(createPlantBlueprint()));

	for (std::string s : PlantBlueprint::getKeys())
	{
		m_ui->m_plantType->addItem(QString::fromStdString(s));
	}
}
//----------------------------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
	delete m_ui;
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::createNewPlant()
{
	const float x = static_cast<float>(m_ui->m_positionX->value());
	const float z = static_cast<float>(m_ui->m_positionZ->value());
	if (m_ui->m_plantType->currentIndex() > -1)
	{
		const std::string type = static_cast<std::string>(m_ui->m_plantType->currentText().toStdString());
		m_gl->createPlant(type,x,z);
	}
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::openPlantBlueprintDialogFromUI()
{
	if (m_ui->m_plantType->currentText() == "create new")
	{
	openPlantBlueprintDialogFromMenubar();
	}
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::openPlantBlueprintDialogFromMenubar()
{
	m_plantBlueprintDialog->show();
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::closePlantBlueprintDialog()
{
	m_plantBlueprintDialog->hide();
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::createPlantBlueprint()
{
	if (m_plantBlueprintDialog->createPlantBlueprint() == true)
	{
		closePlantBlueprintDialog();
		//Update the blueprint names
		for (std::string s : PlantBlueprint::getKeys())
		{
			//Check if the item does not exist
			if (m_ui->m_plantType->findText(QString::fromStdString(s)) == -1)
			{
				m_ui->m_plantType->addItem(QString::fromStdString(s));
			}
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::updatePlants()
{
	m_gl->updatePlants();
}
//----------------------------------------------------------------------------------------------------------------------
