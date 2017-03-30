#include <iostream>
#include <string>
#include <unordered_set>
#include <QString>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "PlantBlueprint.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow)
{
	m_ui->setupUi(this);
	m_gl = new NGLScene(this);
	m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,3,1);

	connect(m_ui->m_updateButton, SIGNAL(released()), m_gl, SLOT(updatePlants()));
	connect(m_ui->m_newPlantButton, SIGNAL(released()), this, SLOT(createNewPlant()));

	std::unordered_set<std::string> keys = PlantBlueprint::getKeys();
	for (std::string s : keys)
	{
		m_ui->m_plantType->addItem(QString::fromStdString(s));
	}
}

MainWindow::~MainWindow()
{
	delete m_ui;
}

void MainWindow::createNewPlant()
{
	const float x = static_cast<float>(m_ui->m_positionX->value());
	const float z = static_cast<float>(m_ui->m_positionZ->value());
	const std::string type = static_cast<std::string>(m_ui->m_plantType->currentText().toStdString());

	m_gl->createPlant(type,x,z);
}
