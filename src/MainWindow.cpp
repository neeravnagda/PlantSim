#include <iostream>
#include <string>
#include <unordered_set>
#include <QApplication>
#include <QString>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ui_PlantBlueprintDialog.h"
#include "ui_LightsDialog.h"
#include "ui_About.h"
#include "PlantBlueprint.h"
//----------------------------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow),
	m_about(new Ui::About)
{
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);

	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,3,1);

	m_plantBlueprintDialog = new PlantBlueprintDialog(this);
	m_lightsDialog = new LightsDialog(this);

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
	//Open the lights dialog
	connect(m_ui->m_lightsButton, SIGNAL(released()), this, SLOT(openLightsDialog()));
	connect(m_ui->s_editLights, SIGNAL(triggered(bool)), this, SLOT(openLightsDialog()));
	//Close the lights dialog
	connect(m_lightsDialog->getUI().m_closeButton, SIGNAL(released()), this, SLOT(closeLightsDialog()));
	//Connect the lights dialog signals
	connect(m_lightsDialog, SIGNAL(lightActive(int,bool)), this, SLOT(setLightActive(int,bool)));
	connect(m_lightsDialog, SIGNAL(positionChanged(int,ngl::Vec3)), this, SLOT(setLightPosition(int,ngl::Vec3)));
	connect(m_lightsDialog, SIGNAL(ambientChanged(int,ngl::Vec3)), this, SLOT(setLightAmbient(int,ngl::Vec3)));
	connect(m_lightsDialog, SIGNAL(diffuseChanged(int,ngl::Vec3)), this, SLOT(setLightDiffuse(int,ngl::Vec3)));
	connect(m_lightsDialog, SIGNAL(specularChanged(int,ngl::Vec3)), this, SLOT(setLightSpecular(int,ngl::Vec3)));

	m_ui->m_plantType->addItem(QString("test"));
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
void MainWindow::openLightsDialog()
{
	m_lightsDialog->show();
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::closeLightsDialog()
{
	m_lightsDialog->hide();
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setLightActive(int _light, bool _status)
{
	m_gl->setLightActive(_light, _status);
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setLightPosition(int _light, ngl::Vec3 _position)
{
	m_gl->setLightPosition(_light, _position);
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setLightAmbient(int _light, ngl::Vec3 _ambient)
{
	m_gl->setLightAmbient(_light, _ambient);
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setLightDiffuse(int _light, ngl::Vec3 _diffuse)
{
	m_gl->setLightDiffuse(_light, _diffuse);
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setLightSpecular(int _light, ngl::Vec3 _specular)
{
	m_gl->setLightSpecular(_light, _specular);
}
//----------------------------------------------------------------------------------------------------------------------
