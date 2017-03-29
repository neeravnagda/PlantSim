#include <QtGui/QGuiApplication>
#include <iostream>
#include <vector>
#include "ngl/Vec3.h"
#include "NGLScene.h"
#include "PlantBlueprint.h"
#include "Plant.h"

int main(int argc, char **argv)
{
/*
		std::vector<Plant> m_plants;

		PlantBlueprint::instance("Test")->readGrammarFromFile("rules.txt");
		PlantBlueprint::instance("Test")->setMaxDepth(5);
		PlantBlueprint::instance("Test")->setDrawLength(1.0f);
		PlantBlueprint::instance("Test")->setDrawAngle(45.0f);

		m_plants.emplace_back("Test",ngl::Vec3::zero());
		std::cout<<m_plants[0].getString()<<" is the first string\n";
		m_plants[0].update();
		std::cout<<m_plants[0].getString()<<" updated\n";

		PlantBlueprint::destroyAll();
		return EXIT_SUCCESS;
*/
  QGuiApplication app(argc, argv);
  // create an OpenGL format specifier
  QSurfaceFormat format;
  // set the number of samples for multisampling
  // will need to enable glEnable(GL_MULTISAMPLE); once we have a context
  format.setSamples(4);
  #if defined( __APPLE__)
    // at present mac osx Mountain Lion only supports GL3.2
    // the new mavericks will have GL 4.x so can change
    format.setMajorVersion(3);
    format.setMinorVersion(3);
  #else
    // with luck we have the latest GL version so set to this
    format.setMajorVersion(3);
    format.setMinorVersion(3);
  #endif
  // now we are going to set to CoreProfile OpenGL so we can't use and old Immediate mode GL
  format.setProfile(QSurfaceFormat::CoreProfile);
  // now set the depth buffer to 24 bits
  format.setDepthBufferSize(24);
  // now we are going to create our scene window
  NGLScene window;
  // and set the OpenGL format
  window.setFormat(format);
  // we can now query the version to see if it worked
  std::cout<<"Profile is "<<format.majorVersion()<<" "<<format.minorVersion()<<"\n";
  // set the window size
  window.resize(1024, 720);
  // and finally show
  window.show();

  return app.exec();

}



