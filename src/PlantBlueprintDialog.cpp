#include <QRegExp>
#include <QRegExpValidator>
#include <QString>
#include <fstream>
#include <string>
#include <unordered_set>
#include "PlantBlueprint.h"
#include "PlantBlueprintDialog.h"
#include "ui_PlantBlueprintDialog.h"
//----------------------------------------------------------------------------------------------------------------------
PlantBlueprintDialog::PlantBlueprintDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::PlantBlueprintDialog)
{
	m_ui->setupUi(this);

	//Set default palette - black text on white background
	c_defaultPalette.setColor(QPalette::Text,Qt::black);
	c_defaultPalette.setColor(QPalette::Base, Qt::white);

	//Set a validator to allow accessing file paths on a Linux terminal
	// (/\\w+)? is an optional "/path"
	// ((../)|(\\w+/))* is an optional number of "../" or "path/"
	// (\\w+\\.) is "file." and is required
	const QString fileRegExp = "(/\\w+)?((../)|(\\w+/))*(\\w+\\.)";

	//Add a validator to the plant blueprint name
	QRegExp pbNameExp("\\w+");//Must be at least one char
	QRegExpValidator *pbNameValidator = new QRegExpValidator(pbNameExp, this);
	m_ui->m_blueprintName->setValidator(pbNameValidator);

	//Add a validator to the QLineEdit for L-system grammar text file
	QString txtFileExp = fileRegExp + "(txt)";
	QRegExp txtFileRegExp(txtFileExp);
	QRegExpValidator *txtFileValidator = new QRegExpValidator(txtFileRegExp, this);
	m_ui->m_grammarFilePath->setValidator(txtFileValidator);

	//Add a validator for the image paths
	QString imageFileExp = fileRegExp + "((exr)|(gif)|(jpeg)|(jpg)|(png)|(tif)|(tiff))";
	QRegExp imageFileRegExp(imageFileExp);
	QRegExpValidator *imageFileValidator = new QRegExpValidator(imageFileRegExp, this);
	m_ui->m_woodTextureFilePath->setValidator(imageFileValidator);
	m_ui->m_leafTextureFilePath->setValidator(imageFileValidator);

	//Connect signals and slots
	connect(m_ui->m_blueprintName, SIGNAL(editingFinished()), this, SLOT(checkBlueprintExists()));
	connect(m_ui->m_blueprintName, SIGNAL(textChanged(QString)), this, SLOT(checkBlueprintExists()));
	connect(m_ui->m_grammarFilePath, SIGNAL(editingFinished()), this, SLOT(checkGrammarFileExists()));
	connect(m_ui->m_grammarFilePath, SIGNAL(textChanged(QString)), this, SLOT(resetGrammarFileTextColour()));
}
//----------------------------------------------------------------------------------------------------------------------
PlantBlueprintDialog::~PlantBlueprintDialog()
{
	delete m_ui;
}
//----------------------------------------------------------------------------------------------------------------------
bool PlantBlueprintDialog::createPlantBlueprint()
{
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
