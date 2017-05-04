#ifndef NGLSCENE_H_
#define NGLSCENE_H_

#include <array>
#include <string>
#include <vector>
#include <ngl/Camera.h>
#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include <QOpenGLWidget>
#include "Plant.h"
#include "WindowParams.h"
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWidget
{
	public:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief ctor for our NGL drawing class
		/// @param [in] parent the parent window to the class
		//----------------------------------------------------------------------------------------------------------------------
		NGLScene(QWidget *_parent);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief dtor must close down ngl and release OpenGL resources
		//----------------------------------------------------------------------------------------------------------------------
		~NGLScene();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief the initialize class is called once when the window is created and we have a valid GL context
		/// use this to setup any default GL stuff
		//----------------------------------------------------------------------------------------------------------------------
		void initializeGL() override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called everytime we want to draw the scene
		//----------------------------------------------------------------------------------------------------------------------
		void paintGL() override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this is called everytime we resize the window
		//----------------------------------------------------------------------------------------------------------------------
		void resizeGL(int _w, int _h) override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Update the plants
		/// This is called from a slot in the MainWindow class
		//----------------------------------------------------------------------------------------------------------------------
		void updatePlants();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Create a new plant object in the container
		/// @param _type The PlantBlueprint for the object to use
		/// @param _x The x coordinate to place the object at
		/// @param _z The z coordinate to place the object at
		//----------------------------------------------------------------------------------------------------------------------
		void createPlant(std::string _type, float _x, float _z);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Set a plant visibility from the container
		/// @param _index The index into the vector m_plants
		/// @param _state The new visibility state
		//----------------------------------------------------------------------------------------------------------------------
		void setPlantVisibility(unsigned _index, bool _state);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Delete a plant from the container
		/// @param _index The index into the vector m_plants
		//----------------------------------------------------------------------------------------------------------------------
		void deletePlant(unsigned _index);

	private:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief windows parameters for mouse control etc.
		//----------------------------------------------------------------------------------------------------------------------
		WinParams m_win;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief position for our model
		/// Since there is more than one model, this acts as the position to rotate around for mouse rotation
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Vec3 m_modelPos;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief used to store global mouse transforms
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Mat4 m_mouseGlobalTX;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief the camera
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Camera m_camera;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Container of plant objects
		//----------------------------------------------------------------------------------------------------------------------
		std::vector<Plant> m_plants;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Draw the ground plane and the plants in the scene
		//----------------------------------------------------------------------------------------------------------------------
		void drawScene();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Qt Event called when a key is pressed
		/// @param [in] _event the Qt event to query for size etc
		//----------------------------------------------------------------------------------------------------------------------
		void keyPressEvent(QKeyEvent *_event) override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this method is called every time a mouse is moved
		/// @param _event the Qt Event structure
		//----------------------------------------------------------------------------------------------------------------------
		void mouseMoveEvent (QMouseEvent * _event ) override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this method is called everytime the mouse button is pressed
		/// inherited from QObject and overridden here.
		/// @param _event the Qt Event structure
		//----------------------------------------------------------------------------------------------------------------------
		void mousePressEvent ( QMouseEvent *_event) override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this method is called everytime the mouse button is released
		/// inherited from QObject and overridden here.
		/// @param _event the Qt Event structure
		//----------------------------------------------------------------------------------------------------------------------
		void mouseReleaseEvent ( QMouseEvent *_event ) override;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief this method is called everytime the mouse wheel is moved
		/// inherited from QObject and overridden here.
		/// @param _event the Qt Event structure
		//----------------------------------------------------------------------------------------------------------------------
		void wheelEvent( QWheelEvent *_event) override;
};

#endif
