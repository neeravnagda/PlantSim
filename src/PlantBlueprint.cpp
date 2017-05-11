#include <algorithm>
#include <fstream>
#include <iostream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <ngl/Texture.h>
#include <ngl/VAOPrimitives.h>
#include "PlantBlueprint.h"
//----------------------------------------------------------------------------------------------------------------------
// Set the static members
std::unordered_map<std::string, PlantBlueprint*> PlantBlueprint::s_instances;
std::unordered_set<std::string> PlantBlueprint::s_keys;
std::string PlantBlueprint::s_shaderProgramName = "Phong";
std::unique_ptr<ngl::Obj> PlantBlueprint::s_cylinder;
std::string PlantBlueprint::s_leafGeometryName = "leafQuad";
GLuint PlantBlueprint::s_leafGeometryTexture;
ngl::Vec3 PlantBlueprint::s_sunPosition = ngl::Vec3(0.0f, 100.0f, 0.0f);
//----------------------------------------------------------------------------------------------------------------------
PlantBlueprint* PlantBlueprint::instance(const std::string _instanceID)
{
	const auto it = s_instances.find(_instanceID);
	//return the instance if it was found
	if (it != s_instances.end())
	{
		return static_cast<PlantBlueprint*>(it->second);
	}
	//If it wasn't found, create a new instance
	PlantBlueprint* blueprint = new PlantBlueprint;
	s_instances[_instanceID] = blueprint;
	s_keys.emplace(_instanceID);
	return blueprint;
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::destroy(const std::string _instanceID)
{
	auto it = s_instances.find(_instanceID);
	if (it != s_instances.end())
	{
		delete (*it).second;
		s_instances.erase(it);
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::destroyAll()
{
	for (auto i : s_instances)
	{
		delete i.second;
	}
	s_instances.clear();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::init()
{
	//Create the geometry
	s_cylinder.reset(new ngl::Obj("models/cylinder_low.obj", "textures/TreeTexture.jpg"));
	s_cylinder->createVAO();

	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	prim->createTrianglePlane(s_leafGeometryName,1,1,1,1,ngl::Vec3::up());

	//Load the shaders
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	shader->createShaderProgram(s_shaderProgramName);
	const std::string vertexShader = "shaders/BlinnPhong.vertex.glsl";
	const std::string fragmentShader = "shaders/BlinnPhong.fragment.glsl";
	shader->loadShader(s_shaderProgramName,vertexShader, fragmentShader);
	//Use the shader
	(*shader)[s_shaderProgramName]->use();

	//Set the leaf texture
	ngl::Texture leafTex("textures/Leaves0203.png");
	s_leafGeometryTexture = leafTex.setTextureGL();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::drawCylinder()
{
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	(*shader)[s_shaderProgramName]->use();
	s_cylinder->draw();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::drawLeaf()
{
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	(*shader)[s_shaderProgramName]->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, s_leafGeometryTexture);

	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();
	prim->draw(s_leafGeometryName);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::setAxiom(const std::string _axiom)
{
	//Convert the axiom to a branch if necessary
	if (_axiom.compare(0,1,"[",1)!=0)
	{
		m_axiom = "[" + _axiom + "]";
	}
	//Otherwise set the axiom as the input
	else
		m_axiom = _axiom;
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::cleanupString(std::string &_string)
{
	//Remove '=' and ','
	_string.erase(std::remove(_string.begin(),_string.end(), '='), _string.end());
	_string.erase(std::remove(_string.begin(),_string.end(), ','), _string.end());
}
//----------------------------------------------------------------------------------------------------------------------
void PlantBlueprint::readGrammarFromFile(const std::string _filePath)
{
	std::ifstream fileIn(_filePath);//Open the file
	std::string line;//Temp string for each line

	//Set regular expressions for valid characters/numbers
	QString validCharacters = "([A-Z]|[" + QRegularExpression::escape("+-/\\\\&^") + "]|(\\[)|(\\]))+";
	QString validNumeric = "(([0-9]+)((\\.)([0-9]+))?)";

	//Set regular expressions using the format "predecessor=successor,probability" or "predecessor=successor"
	QString ruleWithProbabilityString = "^(?<predecessor>" + validCharacters + ")\\=(?<successor>" + validCharacters + ")\\,(?<probability>" + validNumeric + ")$";
	QString ruleWithoutProbabilityString = "^(?<predecessor>" + validCharacters + ")\\=(?<successor>" + validCharacters + ")$";

	//Create the regular expressions
	QRegularExpression ruleWithProbability(ruleWithProbabilityString);
	QRegularExpression ruleWithoutProbability(ruleWithoutProbabilityString);

	//Add the rules, one line at a time
	while (std::getline(fileIn,line))
	{
		//Remove whitespaces
		line.erase(std::remove(line.begin(),line.end(),' '),line.end());

		//Create variables for adding to the container
		std::string predecessorValue;
		std::string successorValue;
		float probabilityValue = 1.0f;

		//Use expression matching to split the string
		QString qLine = QString::fromStdString(line);
		//Check for rule with probability
		QRegularExpressionMatch match = ruleWithProbability.match(qLine);
		if (match.hasMatch())
		{
			predecessorValue = match.captured("predecessor").toStdString();
			successorValue = match.captured("successor").toStdString();
			probabilityValue = match.captured("probability").toFloat();
			std::cout<<"from regex: "<<predecessorValue<<" == "<<successorValue<<", "<<probabilityValue<<"\n";
		}
		//Check for rule without probability
		else
		{
			match = ruleWithoutProbability.match(qLine);
			if (match.hasMatch())
			{
				predecessorValue = match.captured("predecessor").toStdString();
				successorValue = match.captured("successor").toStdString();
				probabilityValue = 1.0f;
				std::cout<<"from regex2: "<<predecessorValue<<" == "<<successorValue<<", "<<probabilityValue<<"\n";
			}
			//The rule was invalid so don't add this line
			else
			{
				continue;
			}
		}

		//Construct the rule in the container
		m_productionRules.emplace_back(predecessorValue,successorValue,probabilityValue);
	}

	std::cout<<"Rules:\n";
	for (ProductionRule r : m_productionRules)
	{
		std::cout<<r.m_predecessor<<" = "<<r.m_successor<< ", "<<r.m_probability<<"\n";
	}
}
//----------------------------------------------------------------------------------------------------------------------
