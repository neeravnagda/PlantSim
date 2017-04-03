#include <iostream>
#include <QMouseEvent>
#include <QGuiApplication>
#include <ngl/NGLInit.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include "NGLScene.h"

NGLScene::NGLScene(QWidget *_parent) : QOpenGLWidget(_parent)
{
	// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
	setFocus();
	this->resize(_parent->size());
}


NGLScene::~NGLScene()
{
	PlantBlueprint::destroyAll();//Clear all Plant Blueprints
	std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::updatePlants()
{
	for (auto &p : m_plants)
	{
		p.update();
	}
	update();
}

void NGLScene::createPlant(std::string _type, float _x, float _z)
{
	m_plants.emplace_back(_type, ngl::Vec3(_x,0.0f,_z));
	update();
}

void NGLScene::resizeGL(int _w , int _h)
{
	m_camera.setShape(45, static_cast<float>(width())/height(), 0.1f, 100.0f);
	m_win.width  = static_cast<int>( _w * devicePixelRatio() );
	m_win.height = static_cast<int>( _h * devicePixelRatio() );
}


void NGLScene::initializeGL()
{
	// we need to initialise the NGL lib which will load all of the OpenGL functions, this must
	// be done once we have a valid GL context but before we call any GL commands. If we dont do
	// this everything will crash
	ngl::NGLInit::instance();
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
	// enable depth testing for drawing
	glEnable(GL_DEPTH_TEST);
	// enable multisampling for smoother drawing
	glEnable(GL_MULTISAMPLE);

	PlantBlueprint::init();

	ngl::Vec3 from = ngl::Vec3(0.0f, 4.0f, -10.0f);
	ngl::Vec3 to = ngl::Vec3::zero();
	ngl::Vec3 up = ngl::Vec3::up();
	m_camera.set(from, to, up);
	m_camera.setShape(45, static_cast<float>(width())/height(), 0.1f, 100.0f);
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();

	shader->setUniform("viewerPos", m_camera.getEye().toVec3());

	std::string activeStatus = "Lights[0].isActive";
	shader->setUniform(activeStatus, true);
	std::string pos = "LightPositions[0]";
	shader->setUniform(pos, ngl::Vec3(-2.0f,2.0f,0.0f));
	std::string ambient = "Lights[0].La";
	shader->setUniform(ambient, ngl::Vec3(1.0f,0.0f,0.0f));
	std::string diffuse = "Lights[0].Ld";
	shader->setUniform(diffuse, ngl::Vec3(1.0f,1.0f,1.0f));
	std::string specular = "Lights[0].Ls";
	shader->setUniform(specular, ngl::Vec3(1.0f,1.0f,1.0f));

	glViewport(0,0,width(),height());

	ngl::VAOPrimitives::instance()->createTrianglePlane("groundPlane", 1, 1, 1, 1, ngl::Vec3::up());
}

void NGLScene::sendUniformsToShader()
{
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();

	ngl::Mat4 rotX;
	ngl::Mat4 rotY;
	rotX.rotateX(m_win.spinXFace);
	rotY.rotateY(m_win.spinYFace);
	m_mouseGlobalTX=rotY*rotX;
	m_mouseGlobalTX.m_30 = m_modelPos.m_x;
	m_mouseGlobalTX.m_31 = m_modelPos.m_y;
	m_mouseGlobalTX.m_32 = m_modelPos.m_z;

	ngl::Mat4 MV;
	ngl::Mat4 MVP;
	ngl::Mat3 N;
	ngl::Mat4 M;
	M *= m_mouseGlobalTX;
	MV = M * m_camera.getViewMatrix();
	MVP = M * m_camera.getProjectionMatrix();
	N = MV;
	N.inverse();
	shader->setUniform("M", M);
	shader->setUniform("MV", MV);
	shader->setUniform("MVP", MVP);
	shader->setUniform("N", N);
}

void NGLScene::drawScene()
{
	sendUniformsToShader();
	ngl::VAOPrimitives::instance()->draw("groundPlane");
	for (Plant &p : m_plants)
	{
		p.draw(m_mouseGlobalTX, m_camera.getViewMatrix(), m_camera.getProjectionMatrix());
	}
}

void NGLScene::paintGL()
{
	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,m_win.width,m_win.height);
	drawScene();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
	// this method is called every time the main window recives a key event.
	// we then switch on the key value and set the camera in the GLWindow
	switch (_event->key())
	{
		// escape key to quit
		//case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
		case Qt::Key_Space :
		{
			m_win.spinXFace=0;
			m_win.spinYFace=0;
			m_modelPos.set(ngl::Vec3::zero());
			break;
		}
		case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
		case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
		default : break;
	}
	// finally update the GLWindow and re-draw

	update();
}
