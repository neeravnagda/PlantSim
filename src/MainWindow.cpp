#include <iostream>
#include <string>
#include <unordered_set>
#include <QApplication>
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
	//Initialise the UI and OpenGL widget (NGLScene)
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);

	//Position the OpenGL widget
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,3,1);

	//Create new dialogs
	m_plantBlueprintDialog = new PlantBlueprintDialog(this);

	//Quit the application
	connect(m_ui->s_quit, SIGNAL(triggered(bool)), this, SLOT(quit()));
	//Update plants
	connect(m_ui->m_updateButton, SIGNAL(released()), this, SLOT(updatePlants()));
	//Create a new plant
	connect(m_ui->m_newPlantButton, SIGNAL(released()), this, SLOT(createNewPlant()));
	//Open the Plant Blueprint dialog
	connect(m_ui->m_plantType, SIGNAL(currentIndexChanged(int)), this, SLOT(openPlantBlueprintDialogFromUI()));
	connect(m_ui->s_newPlantBlueprint, SIGNAL(triggered(bool)), this, SLOT(openPlantBlueprintDialogFromMenubar()));
	//Close the Plant Blueprint dialog
	connect(m_plantBlueprintDialog->getUI().m_cancel, SIGNAL(released()), this, SLOT(closePlantBlueprintDialog()));
	//Create a new Plant Blueprint
	connect(m_plantBlueprintDialog->getUI().m_create, SIGNAL(released()), this, SLOT(createPlantBlueprint()));

	m_ui->m_plantType->addItem("test");

	//Add all the names of the instances of PlantBlueprint
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
void MainWindow::quit()
{
	QApplication::exit(EXIT_SUCCESS);
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::createNewPlant()
{
	//Check if the selection is not blank
	if (m_ui->m_plantType->currentIndex() > 0)
	{
		const float x = static_cast<float>(m_ui->m_positionX->value());
		const float z = static_cast<float>(m_ui->m_positionZ->value());
		const std::string type = m_ui->m_plantType->currentText().toStdString();
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
	//If the validation from PlantBlueprintDialog passed, create a new PlantBlueprint
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
