#include <QtGui/QGuiApplication>
#include <QApplication>
#include <iostream>
#include <vector>
#include "MainWindow.h"
#include "NGLScene.h"

int main(int argc, char **argv)
{
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

	QSurfaceFormat::setDefaultFormat(format);

	QApplication app(argc, argv);
	MainWindow w;

	w.show();

	return app.exec();
}
