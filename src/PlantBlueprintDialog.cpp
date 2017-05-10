#include <unordered_set>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
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
	QRegularExpression pbNameExp("\\w+");//Must be at least one char
	QRegularExpressionValidator *pbNameValidator = new QRegularExpressionValidator(pbNameExp, this);
	m_ui->m_blueprintName->setValidator(pbNameValidator);

	//Add a validator for L-system grammar text file
	QString txtFileExp = fileRegExp + "(txt)";
	QRegularExpression txtFileRegExp(txtFileExp);
	QRegularExpressionValidator *txtFileValidator = new QRegularExpressionValidator(txtFileRegExp, this);
	m_ui->m_grammarFilePath->setValidator(txtFileValidator);

	//Add a validator for the L-system axiom
	//Manual nested branches are used as it is difficult to implement with regular expressions
	QString validCharacters = "([A-Z]*[" + QRegularExpression::escape("+-/\\\\&^") + "]*)*";
	QString branch = "(\\[(" + validCharacters + ")+\\])*";
	QString nest2 = "(\\[(" + validCharacters + branch + ")+\\])*";
	QString nest3 = "(\\[(" + validCharacters + nest2 + ")+\\])*";
	QString axiomPattern = "(" + validCharacters + nest3 + ")+";
	QRegularExpression axiomRegExp(axiomPattern);
	QRegularExpressionValidator *axiomValidator = new QRegularExpressionValidator(axiomRegExp, this);
	m_ui->m_axiom->setValidator(axiomValidator);

	//Connect signals and slots
	connect(m_ui->m_blueprintName, SIGNAL(editingFinished()), this, SLOT(checkBlueprintExists()));
	connect(m_ui->m_blueprintName, SIGNAL(textChanged(QString)), this, SLOT(checkBlueprintExists()));
	connect(m_ui->m_axiom, SIGNAL(textChanged(QString)), this, SLOT(validateAxiom()));
	connect(m_ui->m_grammarFilePath, SIGNAL(editingFinished()), this, SLOT(checkGrammarFileExists()));
	connect(m_ui->m_grammarFilePath, SIGNAL(textChanged(QString)), this, SLOT(resetGrammarFileTextColour()));
	connect(m_ui->m_maxDepth, SIGNAL(valueChanged(int)), this, SLOT(setMaxLeavesStartDepth()));
}
//----------------------------------------------------------------------------------------------------------------------
PlantBlueprintDialog::~PlantBlueprintDialog()
{
	delete m_ui;
}
//----------------------------------------------------------------------------------------------------------------------
bool PlantBlueprintDialog::createPlantBlueprint()
{
	//Return if any validation failed
	for (bool &b : m_validationChecks)
	{
		if (b == false) {return false;}//Exit and do not create a new Plant Blueprint
	}

	//Create a new blueprint and set all the members
	PlantBlueprint *pb = PlantBlueprint::instance(m_ui->m_blueprintName->text().toStdString());
	pb->setAxiom(m_ui->m_axiom->text().toStdString());
	pb->readGrammarFromFile(m_ui->m_grammarFilePath->text().toStdString());
	pb->setMaxDepth(static_cast<unsigned>(m_ui->m_maxDepth->value()));
	pb->setDrawLength(static_cast<float>(m_ui->m_drawLength->value()));
	pb->setDrawAngle(static_cast<float>(m_ui->m_drawAngle->value()));
	pb->setRootRadius(static_cast<float>(m_ui->m_rootRadius->value()));
	pb->setDecay(static_cast<float>(m_ui->m_decay->value()));
	pb->setMaxDeviation(static_cast<float>(m_ui->m_maxDeviation->value()));
	pb->setLeavesPerBranch(static_cast<unsigned>(m_ui->m_leafCount->value()));
	pb->setLeavesStartDepth(static_cast<unsigned>(m_ui->m_leavesStartDepth->value()));
	pb->setLeafScale(static_cast<float>(m_ui->m_leafScale->value()));
	pb->setNodesPerBranch(m_ui->m_numNodes->value());
	pb->setPhototropismScaleFactor(static_cast<float>(m_ui->m_phototropismScaleFactor->value()));
	pb->setGravitropismScaleFactor(static_cast<float>(m_ui->m_gravitropismScaleFactor->value()));

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
		m_validationChecks[VALIDATION::BLUEPRINTNAME] = true;
	}
	else
	{
		palette.setColor(QPalette::Text, Qt::red);//Set text to red if already exists
		m_validationChecks[VALIDATION::BLUEPRINTNAME] = false;
	}
	m_ui->m_blueprintName->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::validateAxiom()
{
	QPalette palette = c_defaultPalette;
	//Set the text colour to green if acceptable
	if (m_ui->m_axiom->hasAcceptableInput())
	{
		bool isOneAlpha = false;
		for (auto &c : m_ui->m_axiom->text())
		{
			if (c.isUpper())
			{
				isOneAlpha = true;
				break;
			}
		}
		if (isOneAlpha)
		{
			palette.setColor(QPalette::Text, Qt::green);
			m_validationChecks[VALIDATION::AXIOM] = true;
		}
	}
	else
	{
		m_validationChecks[VALIDATION::AXIOM] = false;
	}

	//Set the text colour to black if empty string
	if (m_ui->m_axiom->text().isEmpty())
	{
		palette.setColor(QPalette::Text, Qt::black);
		m_validationChecks[VALIDATION::AXIOM] = false;
	}

	m_ui->m_axiom->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::checkGrammarFileExists()
{
	QPalette palette = c_defaultPalette;

	if (checkFileExists(m_ui->m_grammarFilePath->text()))
	{
		palette.setColor(QPalette::Text,Qt::green);//Set text to green if OK
		m_validationChecks[VALIDATION::GRAMMARFILE] = true;
	}
	else
	{
		palette.setColor(QPalette::Text,Qt::red);//Set text to red if invalid
		m_validationChecks[VALIDATION::GRAMMARFILE] = false;
	}
	m_ui->m_grammarFilePath->setPalette(palette);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::resetGrammarFileTextColour()
{
	m_ui->m_grammarFilePath->setPalette(c_defaultPalette);
}
//----------------------------------------------------------------------------------------------------------------------
bool PlantBlueprintDialog::checkFileExists(QString _fileName)
{
	QFileInfo file(_fileName);
	return (file.exists());
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprintDialog::setMaxLeavesStartDepth()
{
	int ld = m_ui->m_leavesStartDepth->value();
	int md = m_ui->m_maxDepth->value();
	m_ui->m_leavesStartDepth->setMaximum(md);
	//reset start depth below max depth
	if (ld > md)
	{
		m_ui->m_leavesStartDepth->setValue(md);
	}
}
//----------------------------------------------------------------------------------------------------------------------
