#include <string>
#include <ngl/ShaderLib.h>
#include "include/LightsDialog.h"
#include "ui_LightsDialog.h"
//----------------------------------------------------------------------------------------------------------------------
LightsDialog::LightsDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::LightsDialog)
{
	m_ui->setupUi(this);

	//Initialise the shader
	//ngl::ShaderLib *shader = ngl::ShaderLib::instance();

/*
	//Initialise the lights in the shader
	for (unsigned i=0; i<m_lights.size(); ++i)
	{
		std::string lightPos = "LightPositions[" + std::to_string(i) + "]";
		shader->setUniform(lightPos, m_lights[i].m_position);

		std::string lights = "Lights[" + std::to_string(i) + "]";
		shader->setUniform(lights+".isActive", m_lights[i].m_isActive);
		shader->setUniform(lights+".La", m_lights[i].m_ambient);
		shader->setUniform(lights+".Ld", m_lights[i].m_diffuse);
		shader->setUniform(lights+".Ls", m_lights[i].m_specular);
	}
*/
	//Connect signals and slots
	connect(m_ui->m_lightSelect, SIGNAL(valueChanged(int)), this, SLOT(changeLightInfo(int)));
	connect(m_ui->m_active, SIGNAL(toggled(bool)), this, SLOT(setActiveStatus()));
	connect(m_ui->m_posX, SIGNAL(valueChanged(double)), this, SLOT(setPos()));
	connect(m_ui->m_posY, SIGNAL(valueChanged(double)), this, SLOT(setPos()));
	connect(m_ui->m_posZ, SIGNAL(valueChanged(double)), this, SLOT(setPos()));
	connect(m_ui->m_ambientR, SIGNAL(valueChanged(double)), this, SLOT(setAmbient()));
	connect(m_ui->m_ambientG, SIGNAL(valueChanged(double)), this, SLOT(setAmbient()));
	connect(m_ui->m_ambientB, SIGNAL(valueChanged(double)), this, SLOT(setAmbient()));
	connect(m_ui->m_diffuseR, SIGNAL(valueChanged(double)), this, SLOT(setDiffuse()));
	connect(m_ui->m_diffuseG, SIGNAL(valueChanged(double)), this, SLOT(setDiffuse()));
	connect(m_ui->m_diffuseB, SIGNAL(valueChanged(double)), this, SLOT(setDiffuse()));
	connect(m_ui->m_specularR, SIGNAL(valueChanged(double)), this, SLOT(setSpecular()));
	connect(m_ui->m_specularG, SIGNAL(valueChanged(double)), this, SLOT(setSpecular()));
	connect(m_ui->m_specularB, SIGNAL(valueChanged(double)), this, SLOT(setSpecular()));
}
//----------------------------------------------------------------------------------------------------------------------
LightsDialog::~LightsDialog()
{
	delete m_ui;
}
//----------------------------------------------------------------------------------------------------------------------
void LightsDialog::changeLightInfo(int _index)
{
	//Set the current index
	m_currentIndex = _index;
	//Set the checkbox
	m_ui->m_active->setChecked(m_lights[_index].m_isActive);
	//Set the position
	m_ui->m_posX->setValue(m_lights[_index].m_position.m_x);
	m_ui->m_posY->setValue(m_lights[_index].m_position.m_y);
	m_ui->m_posZ->setValue(m_lights[_index].m_position.m_z);
	//Set the ambient values
	m_ui->m_ambientR->setValue(m_lights[_index].m_ambient.m_r);
	m_ui->m_ambientG->setValue(m_lights[_index].m_ambient.m_g);
	m_ui->m_ambientB->setValue(m_lights[_index].m_ambient.m_b);
	//Set the diffuse values
	m_ui->m_diffuseR->setValue(m_lights[_index].m_diffuse.m_r);
	m_ui->m_diffuseG->setValue(m_lights[_index].m_diffuse.m_g);
	m_ui->m_diffuseB->setValue(m_lights[_index].m_diffuse.m_b);
	//Set the specular values
	m_ui->m_specularR->setValue(m_lights[_index].m_specular.m_r);
	m_ui->m_specularG->setValue(m_lights[_index].m_specular.m_g);
	m_ui->m_specularB->setValue(m_lights[_index].m_specular.m_b);
}
//----------------------------------------------------------------------------------------------------------------------
void LightsDialog::setActiveStatus()
{
	m_lights[m_currentIndex].m_isActive = m_ui->m_active->isChecked();
	ngl::ShaderLib::instance()->setUniform("Lights["+std::to_string(m_currentIndex)+"].isActive", m_lights[m_currentIndex].m_isActive);
}
//----------------------------------------------------------------------------------------------------------------------
void LightsDialog::setPos()
{
	m_lights[m_currentIndex].m_position.m_x = static_cast<float>(m_ui->m_posX->value());
	m_lights[m_currentIndex].m_position.m_y = static_cast<float>(m_ui->m_posY->value());
	m_lights[m_currentIndex].m_position.m_z = static_cast<float>(m_ui->m_posZ->value());
	ngl::ShaderLib::instance()->setUniform("LightPositions["+std::to_string(m_currentIndex)+"]", m_lights[m_currentIndex].m_position);
}
//----------------------------------------------------------------------------------------------------------------------
void LightsDialog::setAmbient()
{
	m_lights[m_currentIndex].m_ambient.m_r = static_cast<float>(m_ui->m_ambientR->value());
	m_lights[m_currentIndex].m_ambient.m_g = static_cast<float>(m_ui->m_ambientG->value());
	m_lights[m_currentIndex].m_ambient.m_b = static_cast<float>(m_ui->m_ambientB->value());
	ngl::ShaderLib::instance()->setUniform("Lights["+std::to_string(m_currentIndex)+"].La", m_lights[m_currentIndex].m_ambient);
}
//----------------------------------------------------------------------------------------------------------------------
void LightsDialog::setDiffuse()
{
	m_lights[m_currentIndex].m_diffuse.m_r = static_cast<float>(m_ui->m_diffuseR->value());
	m_lights[m_currentIndex].m_diffuse.m_g = static_cast<float>(m_ui->m_diffuseG->value());
	m_lights[m_currentIndex].m_diffuse.m_b = static_cast<float>(m_ui->m_diffuseB->value());
	ngl::ShaderLib::instance()->setUniform("Lights["+std::to_string(m_currentIndex)+"].Ld", m_lights[m_currentIndex].m_diffuse);
}
//----------------------------------------------------------------------------------------------------------------------
void LightsDialog::setSpecular()
{
	m_lights[m_currentIndex].m_specular.m_r = static_cast<float>(m_ui->m_specularR->value());
	m_lights[m_currentIndex].m_specular.m_g = static_cast<float>(m_ui->m_specularG->value());
	m_lights[m_currentIndex].m_specular.m_b = static_cast<float>(m_ui->m_specularB->value());
	ngl::ShaderLib::instance()->setUniform("Lights["+std::to_string(m_currentIndex)+"].Ls", m_lights[m_currentIndex].m_specular);
}
//----------------------------------------------------------------------------------------------------------------------
