#include <QMouseEvent>
#include <QGuiApplication>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include "NGLScene.h"
#include "PlantBlueprint.h"
//----------------------------------------------------------------------------------------------------------------------
NGLScene::NGLScene(QWidget *_parent) : QOpenGLWidget(_parent)
{
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	setFocus();
	this->resize(_parent->size());
}
//----------------------------------------------------------------------------------------------------------------------
NGLScene::~NGLScene(){}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::updatePlants()
{
	//Update all plant simulations
	for (Plant &p : m_plants)
	{
		p.updateSimulation();
	}
	//Force a window update
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::createPlant(std::string _type, float _x, float _z)
{
	ngl::Vec3 pos(_x,0.0f,_z);
	m_plants.emplace_back(_type, pos);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::setPlantVisibility(unsigned _index, bool _state)
{
	m_plants[_index].setVisibility(_state);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::deletePlant(unsigned _index)
{
	m_plants.erase(m_plants.begin() + _index);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::resizeGL(int _w , int _h)
{
	//Set the camera and window parameters
	m_camera.setShape(45, static_cast<float>(width())/height(), 0.001f, 60.0f);
	m_win.width  = static_cast<int>( _w * devicePixelRatio() );
	m_win.height = static_cast<int>( _h * devicePixelRatio() );
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::initializeGL()
{
	// we need to initialise the NGL lib which will load all of the OpenGL functions, this must
	// be done once we have a valid GL context but before we call any GL commands. If we dont do
	// this everything will crash
	ngl::NGLInit::instance();
	glClearColor(0.57f, 0.77f, 0.92f, 1.0f);			   // Light blue background
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);

	// enable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	//Initialise the PlantBlueprint. This initialises the shader and the geometry needed for drawing
	PlantBlueprint::init();

	//Initialise test plant - need to delete this later and set to some presets
	{
		PlantBlueprint *pb = PlantBlueprint::instance("test");
		pb->setAxiom("FA");
		pb->readGrammarFromFile("rules.txt");
		pb->setDecay(1.4f);
		pb->setDrawAngle(45);
		pb->setDrawLength(0.8f);
		pb->setMaxDepth(5);
		pb->setMaxDeviation(0.1f);
		pb->setLeavesPerBranch(30);
		pb->setLeavesStartDepth(2);
		pb->setLeafScale(0.03f);
		pb->setNodesPerBranch(6);
		pb->setRootRadius(0.03f);
		pb->setPhototropismScaleFactor(0.00f);
		pb->setGravitropismScaleFactor(0.0f);
	}

	//Initialise the camera
	m_camera.set(ngl::Vec3(0.0f, 0.6f, 1.6f),//from
							 ngl::Vec3::zero(),//to
							 ngl::Vec3::up());//up
	m_camera.setShape(45, static_cast<float>(width())/height(), 0.001f, 60.0f);

	//Send the viewer position to the shader
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	shader->setUniform("viewerPos", m_camera.getEye().toVec3());

	glViewport(0,0,width(),height());

	//Create the ground plane geometry
	ngl::VAOPrimitives::instance()->createTrianglePlane("groundPlane", 100, 100, 1, 1, ngl::Vec3::up());

	//Initialise the texture for the ground
	ngl::Texture groundTexture("textures/GroundTexture.jpg");
	m_groundTexture = groundTexture.setTextureGL();
	//Make the texture tileable
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::drawScene()
{
	//Calculate the matrices for the ground plane
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	ngl::Mat4 rotX;
	ngl::Mat4 rotY;
	rotX.rotateX(m_win.spinXFace);
	rotY.rotateY(m_win.spinYFace);
	m_mouseGlobalTX=rotY*rotX;
	m_mouseGlobalTX.m_30 = m_modelPos.m_x;
	m_mouseGlobalTX.m_31 = m_modelPos.m_y;
	m_mouseGlobalTX.m_32 = m_modelPos.m_z;
	ngl::Mat4 M;
	M *= m_mouseGlobalTX;
	ngl::Mat4 MV = M * m_camera.getViewMatrix();
	ngl::Mat4 MVP = MV * m_camera.getProjectionMatrix();
	ngl::Mat3 N = MV;
	N.inverse();

	//Send the matrices to the shader
	shader->setUniform("M", M);
	shader->setUniform("MV", MV);
	shader->setUniform("MVP", MVP);
	shader->setUniform("N", N);

	//Set the texture scale as it needs to tile
	shader->setUniform("texScale", 100.0f);

	//Bind the texture for the ground plane
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_groundTexture);

	//Draw the ground plane
	ngl::VAOPrimitives::instance()->draw("groundPlane");

	//Draw the plants if visible
	for (Plant &p : m_plants)
	{
		if (p.getVisibility()) p.draw(m_mouseGlobalTX, m_camera.getViewMatrix(), m_camera.getProjectionMatrix());
	}
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::paintGL()
{
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);
	drawScene();	//Draw the ground plane then call each Plant draw call
}
//----------------------------------------------------------------------------------------------------------------------
