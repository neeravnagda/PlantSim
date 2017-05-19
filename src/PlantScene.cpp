#include <QMouseEvent>
#include <QGuiApplication>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include "PlantScene.h"
#include "PlantBlueprint.h"
//----------------------------------------------------------------------------------------------------------------------
PlantScene::PlantScene(QWidget *_parent) : QOpenGLWidget(_parent)
{
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	setFocus();
	this->resize(_parent->size());
	initialisePresets();
}
//----------------------------------------------------------------------------------------------------------------------
PlantScene::~PlantScene(){}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::initialisePresets()
{
	//Rigid L-system. This has no space colonisation or tropisms
	{
		PlantBlueprint *pb = PlantBlueprint::instance("LSystemClone");
		pb->setAxiom("FFA");
		pb->readGrammarFromFile("presets/LSystemClone.txt");
		pb->setDecay(1.5f);
		pb->setDrawAngle(45);
		pb->setDrawLength(0.7f);
		pb->setMaxDepth(5);
		pb->setMaxDeviation(0.0f);
		pb->setLeavesPerBranch(30);
		pb->setLeavesStartDepth(2);
		pb->setLeafScale(0.05f);
		pb->setControlPointsPerBranch(2);
		pb->setRootRadius(0.1f);
		pb->setPhototropismScaleFactor(0.00f);
		pb->setGravitropismScaleFactor(0.0f);
	}

	//Tangled growth
	{
		PlantBlueprint *pb = PlantBlueprint::instance("TangledBranches");
		pb->setAxiom("FA");
		pb->readGrammarFromFile("presets/TangledBranches.txt");
		pb->setDecay(1.4f);
		pb->setDrawAngle(30);
		pb->setDrawLength(0.8f);
		pb->setMaxDepth(4);
		pb->setMaxDeviation(0.1f);
		pb->setLeavesPerBranch(0);
		pb->setLeavesStartDepth(0);
		pb->setLeafScale(0.03f);
		pb->setControlPointsPerBranch(8);
		pb->setRootRadius(0.05f);
		pb->setPhototropismScaleFactor(0.0f);
		pb->setGravitropismScaleFactor(0.0f);
	}

	//Generic tree
	{
		PlantBlueprint *pb = PlantBlueprint::instance("GenericTree");
		pb->setAxiom("FFA");
		pb->readGrammarFromFile("presets/GenericTree.txt");
		pb->setDecay(1.4f);
		pb->setDrawAngle(45);
		pb->setDrawLength(1.2f);
		pb->setMaxDepth(5);
		pb->setMaxDeviation(0.1f);
		pb->setLeavesPerBranch(30);
		pb->setLeavesStartDepth(3);
		pb->setLeafScale(0.03f);
		pb->setControlPointsPerBranch(6);
		pb->setRootRadius(0.04f);
		pb->setPhototropismScaleFactor(0.005f);
		pb->setGravitropismScaleFactor(0.0f);
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::updatePlants()
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
void PlantScene::createPlant(std::string _type, float _x, float _z)
{
	ngl::Vec3 pos(_x,0.0f,_z);
	m_plants.emplace_back(_type, pos);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::setPlantVisibility(unsigned _index, bool _state)
{
	m_plants[_index].setVisibility(_state);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::deletePlant(unsigned _index)
{
	m_plants.erase(m_plants.begin() + _index);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::resizeGL(int _w , int _h)
{
	//Set the camera and window parameters
	m_camera.setShape(45, static_cast<float>(width())/height(), 0.001f, 60.0f);
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::initializeGL()
{
	QWidget::setFocusPolicy(Qt::StrongFocus);//Set keyboard focus
	ngl::NGLInit::instance();//Initialise the ngl library
	glClearColor(0.57f, 0.77f, 0.92f, 1.0f);// Light blue background
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);

	// enable face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	//Initialise the PlantBlueprint. This initialises the shader and the geometry needed for drawing
	PlantBlueprint::init();

	//Initialise the camera
	m_camera.set(ngl::Vec3(0.0f, 0.5f, 3.0f),//from
							 ngl::Vec3(0.0f, 0.5f, 0.0f),//to
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
void PlantScene::drawScene()
{
	//Calculate the matrices for the ground plane
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	ngl::Mat4 M;
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
		if (p.visibility()) p.draw(m_camera.getViewMatrix(), m_camera.getProjectionMatrix());
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::paintGL()
{
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,width(),height());
	drawScene();	//Draw the ground plane then call each Plant draw call
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::keyPressEvent(QKeyEvent *_event)
{
	switch (_event->key()) {
		//WASD movement
		//Move forward
		case Qt::Key_W:
		{
			ngl::Vec4 forward = m_camera.getN() * -0.05f;
			m_camera.moveBoth(forward.m_x, forward.m_y, forward.m_z);
			break;
		}
		//Move back
		case Qt::Key_S:
		{
			ngl::Vec4 backward = m_camera.getN() * 0.05f;
			m_camera.moveBoth(backward.m_x, backward.m_y, backward.m_z);
			break;
		}
		//Move left
		case Qt::Key_A:
		{
			ngl::Vec4 left = m_camera.getU() * -0.05f;
			m_camera.moveBoth(left.m_x, left.m_y, left.m_z);
			break;
		}
		//Move right
		case Qt::Key_D:
		{
			ngl::Vec4 right = m_camera.getU() * 0.05f;
			m_camera.moveBoth(right.m_x, right.m_y, right.m_z);
			break;
		}
		//Arrow keys movement
		//Move forward
		case Qt::Key_Up:
		{
			ngl::Vec4 forward = m_camera.getN() * -0.05f;
			m_camera.moveBoth(forward.m_x, forward.m_y, forward.m_z);
			break;
		}
		//Move back
		case Qt::Key_Down:
		{
			ngl::Vec4 backward = m_camera.getN() * 0.05f;
			m_camera.moveBoth(backward.m_x, backward.m_y, backward.m_z);
			break;
		}
		//Move left
		case Qt::Key_Left:
		{
			ngl::Vec4 left = m_camera.getU() * -0.05f;
			m_camera.moveBoth(left.m_x, left.m_y, left.m_z);
			break;
		}
		//Move right
		case Qt::Key_Right:
		{
			ngl::Vec4 right = m_camera.getU() * 0.05f;
			m_camera.moveBoth(right.m_x, right.m_y, right.m_z);
			break;
		}
		//Reset the camera
		case Qt::Key_Space:
		{
			m_camera.set(ngl::Vec3(0.0f, 0.5f, 3.0f),//from
									 ngl::Vec3(0.0f, 0.5f, 0.0f),//to
									 ngl::Vec3::up());//up
			break;
		}
		default: break;
	}
	update();
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::mouseMoveEvent( QMouseEvent* _event )
{
	//Rotate the camera
	if ( m_rotate && _event->buttons() == Qt::LeftButton )
	{
		//The mouse movement
		int diffx = _event->x() - m_origX;
		int diffy = _event->y() - m_origY;
		//Calculate a y-rotation
		ngl::Mat4 rotY;
		rotY.rotateY(diffx);
		//Calculate a screen space up rotation
		ngl::Mat4 rotUp;
		rotUp.euler(diffy, m_camera.getU().m_x, m_camera.getU().m_y, m_camera.getU().m_z);
		//Move the look vector to the origin
		ngl::Vec4 look = m_camera.getLook() - m_camera.getEye();
		//Rotate the look vector about the origin
		look = rotY * rotUp * look;
		//Move the look vector back to the transformed position
		look += m_camera.getEye();
		//Set the updated camera
		m_camera.set(m_camera.getEye().toVec3(), look.toVec3(), ngl::Vec3::up());
		//Update the mouse positions
		m_origX = _event->x();
		m_origY = _event->y();
		//Update the window
		update();
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::mousePressEvent( QMouseEvent* _event )
{
	//Set the rotation flag to true
	if ( _event->button() == Qt::LeftButton )
	{
		m_origX  = _event->x();
		m_origY  = _event->y();
		m_rotate = true;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::mouseReleaseEvent( QMouseEvent* _event )
{
	//Set the rotation flag to false
	if ( _event->button() == Qt::LeftButton )
	{
		m_rotate = false;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void PlantScene::wheelEvent( QWheelEvent* _event )
{
	//Calculate a direction vector
	ngl::Vec4 moveDirection = m_camera.getEye() - m_camera.getLook();
	//Normalise and scale the vector by the mouse wheel movement
	moveDirection.normalize();
	moveDirection *= _event->delta() * -0.01f;
	//Move the camera
	m_camera.moveBoth(moveDirection.m_x, moveDirection.m_y, moveDirection.m_z);
	update();
}
//----------------------------------------------------------------------------------------------------------------------
