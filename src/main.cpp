#include <QtGui/QGuiApplication>
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char **argv)
{
	QSurfaceFormat format;
	// set the number of samples for multisampling
	// will need to enable glEnable(GL_MULTISAMPLE); once we have a context
	format.setSamples(4);

	// Set GL version to 3.3
	format.setMajorVersion(3);
	format.setMinorVersion(3);

	// now we are going to set to CoreProfile OpenGL so we can't use and old Immediate mode GL
	format.setProfile(QSurfaceFormat::CoreProfile);
	// now set the depth buffer to 24 bits
	format.setDepthBufferSize(24);

	QSurfaceFormat::setDefaultFormat(format);

	//Create an application and window
	QApplication app(argc, argv);
	MainWindow w;

	//Show the window
	w.show();

	//Return [start] the application
	return app.exec();
}
