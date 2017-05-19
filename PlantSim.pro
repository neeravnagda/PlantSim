#Set some core settings
# Specify the executable name
TARGET=PlantSim
# Default to a console app
CONFIG += console
# Include necessary stuff
QT+=gui opengl core widgets

#Configure folders and file paths
# Executable path
DESTDIR=./
# .o files directory
OBJECTS_DIR=obj
# moc files directory
MOC_DIR=moc
# ui header files directory
UI_DIR=ui
# include directories for search paths
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
    src/Plant.cpp \
    src/PlantBlueprint.cpp \
    src/MainWindow.cpp \
    src/PlantBlueprintDialog.cpp \
    src/SceneManagerDialog.cpp \
    src/PlantScene.cpp

# add .h files
HEADERS+= \
    include/Branch.h \
    include/ProductionRule.h \
    include/Plant.h \
    include/PlantBlueprint.h \
    include/MainWindow.h \
    include/PlantBlueprintDialog.h \
    include/SceneManagerDialog.h \
    include/PlantScene.h

# add the readme, glsl shader files and presets
OTHER_FILES+= README.md \
		shaders/BlinnPhong.fragment.glsl \
		shaders/BlinnPhong.vertex.glsl \
    presets/*

# add the ui forms
FORMS += ui/MainWindow.ui \
    ui/PlantBlueprintDialog.ui \
    ui/SceneManagerDialog.ui

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
