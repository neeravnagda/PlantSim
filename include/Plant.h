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
/// @brief This class simulates one Plant
/// @author Neerav Nagda
/// @version 0.1
/// @date 01/04/17
/// @class Plant
/// @brief This class manages the simulation of one plant
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
		/// @brief Update function to evaluate the Plant simulation
		/// This expands the L-system, converts the string to tokens, and adds bounding boxes to a R-tree
		//----------------------------------------------------------------------------------------------------------------------
		void update();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Draw the plant
		/// @param _mouseGlobalTX The global mouse transformation
		/// @param _viewMatrix The view matrix from the camera
		/// @param _projectionMatrix The projection matrix from the camera
		//----------------------------------------------------------------------------------------------------------------------
		void draw(ngl::Mat4 _mouseGlobalTX, ngl::Mat4 _viewMatrix, ngl::Mat4 _projectionMatrix);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get function for the L-system string
		/// @return The L-system string
		//----------------------------------------------------------------------------------------------------------------------
		std::string getString() const {return m_string;}

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
		/// @brief Generate a random number for stochastic L-systems
		/// @return Random float in the range [0,1]
		//----------------------------------------------------------------------------------------------------------------------
		float genRand();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate a scalar decay value
		/// @return Float in the range [0,1]
		/// This is used to shorten branches, and must be multiplied to a length
		//----------------------------------------------------------------------------------------------------------------------
		float calculateDecay(unsigned _depth);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Expand the string using the L-system rules
		//----------------------------------------------------------------------------------------------------------------------
		void stringRewrite();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Count the number of branches in a given string
		/// @param _string A valid L-system string
		/// @return The number of branches within the string
		//----------------------------------------------------------------------------------------------------------------------
		unsigned countBranches(std::string _string);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Add to the container m_branches
		/// @param _number The number of new branches to add
		/// @param _position The position in the container
		/// This creates the struct branch with only two of the four attributes
		//----------------------------------------------------------------------------------------------------------------------
		void addBranches(unsigned _number, unsigned _position);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Split the L-system string to branches
		/// This adds the string member to each branch struct in the container m_branches
		//----------------------------------------------------------------------------------------------------------------------
		void stringToBranches();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Convert branch structs back to one single string
		//----------------------------------------------------------------------------------------------------------------------
		void branchesToString();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Evaluate branches to add end positions
		/// This is calculated once per L-system update for collision detection
		//----------------------------------------------------------------------------------------------------------------------
		void evaluateBranches();
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
};

#endif // PLANT_H_
