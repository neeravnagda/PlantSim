#include <iostream>
#include <cmath>
#include <QMouseEvent>
#include <QGuiApplication>

#include <ngl/Light.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include "NGLScene.h"
#include "PlantBlueprint.h"

NGLScene::NGLScene()
{
		// re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
		setTitle("PlantSim");

		PlantBlueprint* pb = PlantBlueprint::instance("test");
		pb->readGrammarFromFile("rules.txt");
		pb->setMaxDepth(5);
		pb->setDrawLength(1.0f);
		pb->setDrawAngle(45.0f);
		pb->setRootRadius(0.1f);
		pb->setDecay(DecayType::LINEAR);

		m_plants.emplace_back("test",ngl::Vec3::zero());
		m_plants[0].update();
		std::cout<<m_plants[0].getString()<<" updated\n";
		m_plants[0].update();
		std::cout<<m_plants[0].getString()<<" updated\n";
}


NGLScene::~NGLScene()
{
		std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
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

		PlantBlueprint *pb = PlantBlueprint::instance("test");
		pb->createShaderProgram("Phong");
		pb->loadShader("shaders/PhongVertex.glsl", ngl::ShaderType::VERTEX);
		pb->loadShader("shaders/PhongFragment.glsl", ngl::ShaderType::FRAGMENT);
		pb->linkProgram();

		ngl::ShaderLib* shader = ngl::ShaderLib::instance();
		(*shader)[ pb->getShaderProgramName() ]->use();
		shader->setShaderParam1i("Normalize",1);
		ngl::Material m(ngl::STDMAT::GOLD);
		m.loadToShader("material");

		ngl::Vec3 from = ngl::Vec3(0.0f, 4.0f, -10.0f);
		ngl::Vec3 to = ngl::Vec3::zero();
		ngl::Vec3 up = ngl::Vec3::up();
		m_camera.set(from, to, up);

		m_camera.setShape(45, static_cast<float>(width())/height(), 0.1f, 100.0f);
		shader->setShaderParam3f("viewerPos", m_camera.getEye().m_x, m_camera.getEye().m_y, m_camera.getEye().m_z);

		ngl::Real lightAngle = 0.0f;
		m_light.reset( new ngl::Light(ngl::Vec3(sin(lightAngle), 2, cos(lightAngle)),
																	ngl::Colour(1,1,1,1),
																	ngl::Colour(1,1,1,1),
																	ngl::LightModes::POINTLIGHT));

		ngl::Mat4 iv = m_camera.getViewMatrix();
		iv.transpose();
		m_light->setTransform(iv);
		m_light->loadToShader("light");

		PlantBlueprint::initGeometry();
		glViewport(0,0,width(),height());
}

void NGLScene::drawScene()
{
		ngl::Mat4 rotX;
		ngl::Mat4 rotY;
		rotX.rotateX(m_win.spinXFace);
		rotY.rotateY(m_win.spinYFace);
		m_mouseGlobalTX=rotY*rotX;
		m_mouseGlobalTX.m_30 = m_modelPos.m_x;
		m_mouseGlobalTX.m_31 = m_modelPos.m_y;
		m_mouseGlobalTX.m_32 = m_modelPos.m_z;

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
				// escape key to quite
				case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
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
