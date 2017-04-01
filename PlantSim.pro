#Set some core settings
# specify the executable name
TARGET=PlantSim
# were are going to default to a console app
#CONFIG += console
# core Qt Libs to use add more here if needed.
QT+=gui opengl core

#Configure folders and file paths
# where our exe is going to live (root of project)
DESTDIR=./
# where to put the .o files
OBJECTS_DIR=obj
# where to put moc auto generated files
MOC_DIR=moc
# where to put ui header files
UI_DIR=ui
# and add the include dir into the search path for Qt and make
INCLUDEPATH +=include \
			    ui

#Set some mac stuff
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
	cache()
	DEFINES +=QT5BUILD
}
# on a mac we don't create a .app bundle file ( for ease of multiplatform use)
CONFIG-=app_bundle

# add .cpp files
SOURCES+= src/main.cpp \
	  src/NGLScene.cpp \
	  src/NGLSceneMouseControls.cpp \
    src/Plant.cpp \
    src/PlantBlueprint.cpp \
    src/MainWindow.cpp \
    src/PlantBlueprintDialog.cpp

# add .h files
HEADERS+= include/NGLScene.h \
	  include/WindowParams.h \
    include/Branch.h \
    include/ProductionRule.h \
    include/RTreeTypes.h \
    include/Plant.h \
    include/PlantBlueprint.h \
    include/MainWindow.h \
    include/PlantBlueprintDialog.h

# add the readme and glsl shader files
OTHER_FILES+= README.md \
                          shaders/*.glsl \
                          rules.txt

# add the ui form
FORMS += ui/MainWindow.ui \
    ui/PlantBlueprintDialog.ui

#Sort out NGL stuff
NGLPATH=$$(NGLDIR)
isEmpty(NGLPATH){ # note brace must be here
	message("including $HOME/NGL")
	include($(HOME)/NGL/UseNGL.pri)
}
else{ # note brace must be here
	message("Using custom NGL location")
	include($(NGLDIR)/UseNGL.pri)
}

DISTFILES += \
    shaders/Phong.vertex.glsl \
    shaders/Phong.fragment.glsl
