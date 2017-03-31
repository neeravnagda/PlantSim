#include <QLabel>
#include <QPalette>
#include <QRegExp>
#include <QRegExpValidator>
#include <QString>
#include <fstream>
#include <string>
#include <unordered_set>
#include <ngl/Shader.h>
#include "PlantBlueprint.h"
#include "PlantBlueprintDialog.h"
#include "ui_PlantBlueprintDialog.h"
//----------------------------------------------------------------------------------------------------------------------
PlantBlueprintDialog::PlantBlueprintDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PlantBlueprintDialog)
{
	m_ui->setupUi(this);
	updateShaders();

	//Create widgets
	m_shaderNameWidget = new QLineEdit(this);
	m_vertexShaderPathWidget = new QLineEdit(this);
	m_fragmentShaderPathWidget = new QLineEdit(this);
	m_shaderPathsLabel = new QLabel(this);
	//Add widgets to the UI
	m_ui->s_gridLayout->addWidget(m_shaderNameWidget,16,0,1,2);
	m_ui->s_gridLayout->addWidget(m_shaderPathsLabel,17,0,1,2);
	m_ui->s_gridLayout->addWidget(m_vertexShaderPathWidget,18,0,1,2);
	m_ui->s_gridLayout->addWidget(m_fragmentShaderPathWidget,19,0,1,2);
	//Initially hide them
	m_shaderNameWidget->hide();
	m_vertexShaderPathWidget->hide();
	m_fragmentShaderPathWidget->hide();
	m_shaderPathsLabel->hide();
	//Initialise widget text
	m_shaderPathsLabel->setText("Shader Paths");
	m_shaderNameWidget->setToolTip("New Shader Name");
	m_shaderNameWidget->setPlaceholderText("Shader Name");
	m_vertexShaderPathWidget->setToolTip("Vertex Shader File Path");
	m_vertexShaderPathWidget->setPlaceholderText("Vertex Shader File");
	m_fragmentShaderPathWidget->setToolTip("Fragment Shader File Path");
	m_fragmentShaderPathWidget->setPlaceholderText("Fragment Shader File");

	//Set default palette - black text on white background
	c_defaultPalette.setColor(QPalette::Text,Qt::black);
	c_defaultPalette.setColor(QPalette::Base, Qt::white);

	//Add a validator to the plant blueprint name
	QRegExp pbNameExp("\\w+");//Must be at least one char
	QRegExpValidator *pbNameValidator = new QRegExpValidator(pbNameExp, this);
	m_ui->m_blueprintName->setValidator(pbNameValidator);

	//Add a validator to the QLineEdit for L-system grammar text file
	// follows strict pattern "../path/" + "path/" + "filename.txt"
	// only "filename.txt" is mandatory
	// optional number of "../path/" and "path/"
	QRegExp txtFileExp("(../\\w+/)*(\\w+/)*(\\w+\\.txt)");
	QRegExpValidator *txtFileValidator = new QRegExpValidator(txtFileExp, this);
	m_ui->m_grammarFilePath->setValidator(txtFileValidator);

	//Add a validator for the QLineEdit for shader name QLineEdit
	QRegExp shaderNameExp("\\w+");//Must be at least one char
	QRegExpValidator *shaderNameValidator = new QRegExpValidator(shaderNameExp, this);
	m_shaderNameWidget->setValidator(shaderNameValidator);

	//Add a validator for the QLineEdits for shader files
	QRegExp vertexShaderFileExp("(../\\w+/)*(\\w+/)*(\\w+)((\\.vs)|(\\.vert)|(\\.vertex))?(\\.glsl)");
	QRegExpValidator *vertexShaderFileValidator = new QRegExpValidator(vertexShaderFileExp, this);
	m_vertexShaderPathWidget->setValidator(vertexShaderFileValidator);

	QRegExp fragmentShaderFileExp("(../\\w+/)*(\\w+/)*(\\w+)((\\.fs)|(\\.frag)|(\\.fragment))?(\\.glsl)");
	QRegExpValidator *fragmentShaderFileValidator = new QRegExpValidator(fragmentShaderFileExp, this);
	m_fragmentShaderPathWidget->setValidator(fragmentShaderFileValidator);

	//Connect signals and slots
	connect(m_ui->m_blueprintName, SIGNAL(editingFinished()), this, SLOT(checkBlueprintExists()));
	connect(m_ui->m_blueprintName, SIGNAL(textChanged(QString)), this, SLOT(checkBlueprintExists()));
	connect(m_ui->m_grammarFilePath, SIGNAL(editingFinished()), this, SLOT(checkGrammarFileExists()));
	connect(m_ui->m_grammarFilePath, SIGNAL(textChanged(QString)), this, SLOT(resetGrammarFileTextColour()));
	connect(m_ui->m_shaderSelect, SIGNAL(currentIndexChanged(int)), this, SLOT(updateShaderWidgets()));
	connect(m_shaderNameWidget, SIGNAL(editingFinished()), this, SLOT(checkShaderNameExists()));
	connect(m_shaderNameWidget, SIGNAL(textEdited(QString)), this, SLOT(checkShaderNameExists()));
	connect(m_vertexShaderPathWidget, SIGNAL(editingFinished()), this, SLOT(checkVertexShaderExists()));
	connect(m_vertexShaderPathWidget, SIGNAL(textChanged(QString)), this, SLOT(resetVertexShaderTextColour()));
	connect(m_fragmentShaderPathWidget, SIGNAL(editingFinished()), this, SLOT(checkFragmentShaderExists()));
	connect(m_fragmentShaderPathWidget, SIGNAL(textChanged(QString)), this, SLOT(resetFragmentShaderTextColour()));
}
//----------------------------------------------------------------------------------------------------------------------
PlantBlueprintDialog::~PlantBlueprintDialog()
{
	delete m_ui;
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::updateShaderWidgets()
{
	if ((m_ui->m_shaderSelect->currentText() == "create new") && (m_isNewShaderWidgets == false))
	{
		addNewShaderWidgets();
		m_isNewShaderWidgets = true;
	}
	else if ((m_ui->m_shaderSelect->currentText() != "create new") && (m_isNewShaderWidgets == true))
	{
		removeNewShaderWidgets();
		m_isNewShaderWidgets = false;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::addNewShaderWidgets()
{
	m_shaderNameWidget->show();
	m_shaderPathsLabel->show();
	m_vertexShaderPathWidget->show();
	m_fragmentShaderPathWidget->show();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::removeNewShaderWidgets()
{
	m_shaderNameWidget->hide();
	m_shaderPathsLabel->hide();
	m_vertexShaderPathWidget->hide();
	m_fragmentShaderPathWidget->hide();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::updateShaders()
{
	//Update items if necessary
	for (auto &s : PlantBlueprint::getShaderNames())
	{
		//Check if the item does not exist
		if (m_ui->m_shaderSelect->findText(QString::fromStdString(s)) == -1)
		{
			m_ui->m_shaderSelect->addItem(QString::fromStdString(s));
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
bool PlantBlueprintDialog::createPlantBlueprint()
{
	if (m_ui->m_shaderSelect->currentText() != "create new")
	{
		m_validationChecks[ValidationVariables::SHADERNAME] = true;
		m_validationChecks[ValidationVariables::VERTEXSHADER] = true;
		m_validationChecks[ValidationVariables::FRAGMENTSHADER] = true;
	}
	for (bool &b : m_validationChecks)
	{
		if (b == false) {return false;}//Exit and do not create a new Plant Blueprint
	}
	PlantBlueprint *pb = PlantBlueprint::instance(m_ui->m_blueprintName->text().toStdString());
	pb->readGrammarFromFile(m_ui->m_grammarFilePath->text().toStdString());
	pb->setMaxDepth(m_ui->m_maxDepth->value());
	pb->setDrawLength(static_cast<float>(m_ui->m_drawLength->value()));
	pb->setDrawAngle(static_cast<float>(m_ui->m_drawAngle->value()));
	pb->setRootRadius(static_cast<float>(m_ui->m_rootRadius->value()));
	pb->setDecay(static_cast<float>(m_ui->m_decay->value()));

	if (m_ui->m_shaderSelect->currentText() == "create new")
	{
		pb->createShaderProgram(m_shaderNameWidget->text().toStdString());
		pb->loadShader(m_vertexShaderPathWidget->text().toStdString(), ngl::ShaderType::VERTEX);
		pb->loadShader(m_fragmentShaderPathWidget->text().toStdString(), ngl::ShaderType::FRAGMENT);
		pb->linkProgram();
	}
	else
	{
		pb->setShaderProgram(m_ui->m_shaderSelect->currentText().toStdString());
	}
	return true;
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::checkBlueprintExists()
{
	QPalette palette = c_defaultPalette;
	auto check = PlantBlueprint::getKeys().find(m_ui->m_blueprintName->text().toStdString());
	if (m_ui->m_blueprintName->text().length() == 0)
	{
		m_ui->m_blueprintName->setPalette(palette);//Set text to black if empty
	}
	else if (check == PlantBlueprint::getKeys().end())
	{
		palette.setColor(QPalette::Text, Qt::green);//Set text to green if OK
		m_validationChecks[ValidationVariables::BLUEPRINTNAME] = true;
	}
	else
	{
		palette.setColor(QPalette::Text, Qt::red);//Set text to red if already exists
		m_validationChecks[ValidationVariables::BLUEPRINTNAME] = false;
	}
	m_ui->m_blueprintName->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::checkGrammarFileExists()
{
	QPalette palette = c_defaultPalette;

	std::ifstream file(m_ui->m_grammarFilePath->text().toStdString(), std::ifstream::in);//Open as read-only
	if (file.is_open())//Check if the file opened
	{
		palette.setColor(QPalette::Text,Qt::green);//Set text to green if OK
		m_validationChecks[ValidationVariables::GRAMMARFILE] = true;
	}
	else
	{
		palette.setColor(QPalette::Text,Qt::red);//Set text to red if invalid
		m_validationChecks[ValidationVariables::GRAMMARFILE] = false;
	}
	m_ui->m_grammarFilePath->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::resetGrammarFileTextColour()
{
	m_ui->m_grammarFilePath->setPalette(c_defaultPalette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::checkShaderNameExists()
{
	QPalette palette = c_defaultPalette;
	auto check = PlantBlueprint::getShaderNames().find(m_shaderNameWidget->text().toStdString());
	if (m_shaderNameWidget->text().length() == 0)
	{
		m_shaderNameWidget->setPalette(palette);//Set text to black if empty
	}
	else if (check == PlantBlueprint::getShaderNames().end())
	{
		palette.setColor(QPalette::Text, Qt::green);//Set text to green if OK
		m_validationChecks[ValidationVariables::SHADERNAME] = true;
	}
	else
	{
		palette.setColor(QPalette::Text, Qt::red);//Set text to red if already exists
		m_validationChecks[ValidationVariables::SHADERNAME] = false;
	}
	m_shaderNameWidget->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::checkVertexShaderExists()
{
	QPalette palette = c_defaultPalette;

	std::ifstream file(m_vertexShaderPathWidget->text().toStdString(), std::ifstream::in);//Open as read-only
	if (file.is_open())//Check if the file opened
	{
		palette.setColor(QPalette::Text,Qt::green);//Set text to green if OK
		m_validationChecks[ValidationVariables::VERTEXSHADER] = true;
	}
	else
	{
		palette.setColor(QPalette::Text,Qt::red);//Set text to red if invalid
		m_validationChecks[ValidationVariables::VERTEXSHADER] = false;
	}
	m_vertexShaderPathWidget->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::resetVertexShaderTextColour()
{
	m_vertexShaderPathWidget->setPalette(c_defaultPalette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::checkFragmentShaderExists()
{
	QPalette palette = c_defaultPalette;

	std::ifstream file(m_fragmentShaderPathWidget->text().toStdString(), std::ifstream::in);//Open as read-only
	if (file.is_open())//Check if the file opened
	{
		palette.setColor(QPalette::Text,Qt::green);//Set text to green if OK
		m_validationChecks[ValidationVariables::FRAGMENTSHADER] = true;
	}
	else
	{
		palette.setColor(QPalette::Text,Qt::red);//Set text to red if invalid
		m_validationChecks[ValidationVariables::FRAGMENTSHADER] = false;
	}
	m_fragmentShaderPathWidget->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::resetFragmentShaderTextColour()
{
	m_fragmentShaderPathWidget->setPalette(c_defaultPalette);
}
//----------------------------------------------------------------------------------------------------------------------
