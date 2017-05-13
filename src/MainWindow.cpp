#include <string>
#include <unordered_set>
#include <QApplication>
#include <QString>
#include "MainWindow.h"
#include "PlantBlueprint.h"
#include "ui_MainWindow.h"
#include "ui_PlantBlueprintDialog.h"
#include "ui_SceneManagerDialog.h"
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
	m_sceneManagerDialog = new SceneManagerDialog(this);

	//Quit the application
	connect(m_ui->s_quit, SIGNAL(triggered(bool)), this, SLOT(quit()));
	//Update plants
	connect(m_ui->m_updateButton, SIGNAL(released()), this, SLOT(updatePlants()));
	//Create a new plant
	connect(m_ui->m_newPlantButton, SIGNAL(released()), this, SLOT(createNewPlant()));
	//Delete a plant
	connect(m_sceneManagerDialog, SIGNAL(deletePlantSignal(uint)), this, SLOT(deletePlant(uint)));
	//Open the Plant Blueprint dialog
	connect(m_ui->m_plantType, SIGNAL(currentIndexChanged(int)), this, SLOT(openPlantBlueprintDialogFromUI()));
	connect(m_ui->s_newPlantBlueprint, SIGNAL(triggered(bool)), this, SLOT(openPlantBlueprintDialogFromMenubar()));
	//Close the Plant Blueprint dialog
	connect(m_plantBlueprintDialog->getUI().m_cancel, SIGNAL(released()), this, SLOT(closePlantBlueprintDialog()));
	//Create a new Plant Blueprint
	connect(m_plantBlueprintDialog->getUI().m_create, SIGNAL(released()), this, SLOT(createPlantBlueprint()));
	//Open the scene manager
	connect(m_ui->s_sceneManagerMenuButton, SIGNAL(triggered(bool)), this, SLOT(openSceneManager()));
	connect(m_ui->m_sceneManagerButton, SIGNAL(released()), this, SLOT(openSceneManager()));
	//Close the scene manager
	connect(m_sceneManagerDialog->getUI().m_closeButton, SIGNAL(released()), this, SLOT(closeSceneManager()));
	//Set the plant visibility
	connect(m_sceneManagerDialog, SIGNAL(plantVisibility(uint,bool)), this, SLOT(setPlantVisibility(uint,bool)));

	//Add all preset values
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
	//If the selection is for create new and the dialog is currently closed, show the dialog
	if ((m_ui->m_plantType->currentText() == "create new") && (m_plantBlueprintDialog->isHidden()))
	{
		m_plantBlueprintDialog->show();
	}

	//Check if the selection is not blank
	if (m_ui->m_plantType->currentIndex() > 0)
	{
		//Create a new plant with the parameters
		const float x = static_cast<float>(m_ui->m_positionX->value());
		const float z = static_cast<float>(m_ui->m_positionZ->value());
		const QString type = m_ui->m_plantType->currentText();
		m_gl->createPlant(type.toStdString(),x,z);
		m_sceneManagerDialog->addRow(type, x, 0, z);
	}
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::deletePlant(unsigned _index)
{
	m_gl->deletePlant(_index);
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
void MainWindow::openSceneManager()
{
	m_sceneManagerDialog->show();
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::closeSceneManager()
{
	m_sceneManagerDialog->hide();
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
			//Check if the item does not exist before adding it
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
	//Update the plant simulations
	m_gl->updatePlants();
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::setPlantVisibility(unsigned _index, bool _state)
{
	m_gl->setPlantVisibility(_index, _state);
}
//----------------------------------------------------------------------------------------------------------------------
void MainWindow::keyPressEvent(QKeyEvent *_event)
{
	switch (_event->key())
	{
		case Qt::Key_Escape: quit(); break;
		default: break;
	}
}
//----------------------------------------------------------------------------------------------------------------------
