#include "include/MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		m_ui(new Ui::MainWindow)
{
		m_ui->setupUi(this);
		m_gl = new NGLScene(this);
		m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,3,1);

		connect(m_ui->m_updateButton, SIGNAL(released()), m_gl, SLOT(updatePlants()));
}

MainWindow::~MainWindow()
{
		delete m_ui;
}
