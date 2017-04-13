#ifndef PLANT_H_
#define PLANT_H_

#include <random>
#include <string>
#include <vector>
#include <ngl/Mat4.h>
#include <ngl/Vec3.h>
#include "Branch.h"
#include "PlantBlueprint.h"
#include "ProductionRule.h"
#include "RTreeTypes.h"

//----------------------------------------------------------------------------------------------------------------------
/// @file Plant.h
/// @brief This class simulates and draws one Plant
/// @author Neerav Nagda
/// @version 1.0
/// @date 13/04/17
/// @class Plant
/// @brief This class manages the simulation and drawing of one plant
//----------------------------------------------------------------------------------------------------------------------

class Plant
{
	public:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Constructor for the class
		/// @param _blueprint The name of the PlantBlueprint that this object uses
		/// @param _position The position of the Plant on the ground. Note that the y coordinate is always 0 to be on the ground
		//----------------------------------------------------------------------------------------------------------------------
		Plant(std::string _blueprint, ngl::Vec3 _position);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Destructor
		//----------------------------------------------------------------------------------------------------------------------
		~Plant();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Draw the plant
		/// @param _mouseGlobalTX The global mouse transformation
		/// @param _viewMatrix The view matrix from the camera
		/// @param _projectionMatrix The projection matrix from the camera
		//----------------------------------------------------------------------------------------------------------------------
		void draw(ngl::Mat4 _mouseGlobalTX, ngl::Mat4 _viewMatrix, ngl::Mat4 _projectionMatrix);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Update function to evaluate the Plant simulation
		//----------------------------------------------------------------------------------------------------------------------
		void update();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get function for the L-system string
		/// @return The L-system string
		//----------------------------------------------------------------------------------------------------------------------
		const std::string& getString() {return m_string;}

	private:
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The PlantBlueprint containing Plant information
		//----------------------------------------------------------------------------------------------------------------------
		PlantBlueprint* m_blueprint;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The position of the object on the ground
		/// The y coordinate is always 0 to be on the ground
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Vec3 m_position;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Static seed for random number generator
		//----------------------------------------------------------------------------------------------------------------------
		static std::random_device s_randomDevice;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Static mersenne twister algorithm
		//----------------------------------------------------------------------------------------------------------------------
		static std::mt19937 s_numberGenerator;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The L-system string
		//----------------------------------------------------------------------------------------------------------------------
		std::string m_string;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief The current depth of the L-system string expansion
		//----------------------------------------------------------------------------------------------------------------------
		unsigned m_depth = 0;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Container for branch information
		//----------------------------------------------------------------------------------------------------------------------
		std::vector<Branch> m_branches;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Transformation for draw calls
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Mat4 m_transform;

		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate and load the matrices to the shader
		/// @param _mouseGlobalTX The global mouse transformation
		/// @param _viewMatrix The view matrix from the camera
		/// @param _projectionMatrix The projection matrix from the camera
		//----------------------------------------------------------------------------------------------------------------------
		void loadMatricesToShader(ngl::Mat4 _mouseGlobalTX, ngl::Mat4 _viewMatrix, ngl::Mat4 _projectionMatrix);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate axis-angle rotation matrix
		/// @param _angle The angle to rotate in radians
		/// @param _axis The axis to rotate around
		/// This is a function "euler" in ngl::Mat4, but this is reimplemented to avoid converting from radians to degrees and back to radians
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Mat4 axisAngleRotationMatrix(float _angle, ngl::Vec3 _axis); //whatever im fabulous. idgaf
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Generate a random number for stochastic L-systems
		/// @return Random float in the range [0,1]
		//----------------------------------------------------------------------------------------------------------------------
		float genRand();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Count the number of branches in a string
		/// @param _string The string to count branches from
		/// @return The number of branches in the string
		//----------------------------------------------------------------------------------------------------------------------
		unsigned countBranches(std::string _string);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Add branches to the container
		/// @param _number The number of branches to add
		/// @param _position The position in the container to add at
		//----------------------------------------------------------------------------------------------------------------------
		void addBranches(unsigned _number, unsigned _position);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Convert the string to branches
		/// This is used once to initialise the container of branches,
		/// and once per update to populate the strings in each struct
		//----------------------------------------------------------------------------------------------------------------------
		void stringToBranches();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Expand the string using the L-system rules
		//----------------------------------------------------------------------------------------------------------------------
		void stringRewrite();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate a scalar decay value
		/// @return Float in the range [0,1]
		/// This is used to shorten branches, and must be multiplied to a length
		//----------------------------------------------------------------------------------------------------------------------
		float calculateDecay(unsigned _depth);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate the branch positions using space colonisation
		/// @param _branch A reference to the branch to calculate
		/// @param _direction The direction to perform space colonisation in
		/// This is used to shorten branches, and must be multiplied to a length
		//----------------------------------------------------------------------------------------------------------------------
		void spaceColonisation(Branch &_branch, ngl::Vec3 &_direction);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Evaluate the L-system string to calculate branches
		/// This is calculated once per L-system update as it is an expensive operation
		//----------------------------------------------------------------------------------------------------------------------
		void evaluateBranches();

};

#endif // PLANT_H_
