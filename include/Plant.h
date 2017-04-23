#ifndef PLANT_H_
#define PLANT_H_

#include <random>
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
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
		Plant(const std::string& _blueprint, const ngl::Vec3& _position);
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
		void draw(const ngl::Mat4& _mouseGlobalTX, const ngl::Mat4 _viewMatrix, const ngl::Mat4 _projectionMatrix);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Update function to evaluate the Plant simulation
		//----------------------------------------------------------------------------------------------------------------------
		void update();
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Get function for the L-system string
		/// @return The L-system string
		//----------------------------------------------------------------------------------------------------------------------
		const std::string& getString() const {return m_string;}

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
		/// @brief ID generator for branches
		//----------------------------------------------------------------------------------------------------------------------
		static boost::uuids::random_generator s_IDGenerator;
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
		void loadMatricesToShader(const ngl::Mat4& _mouseGlobalTX, const ngl::Mat4 _viewMatrix, const ngl::Mat4 _projectionMatrix) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate axis-angle rotation matrix
		/// @param _angle The angle to rotate in radians
		/// @param _axis The axis to rotate around
		/// This is a function "euler" in ngl::Mat4, but this is reimplemented to avoid converting from radians to degrees and back to radians
		/// Since this is called multiple times in one draw call, it saves a lot of divide operations
		//----------------------------------------------------------------------------------------------------------------------
		ngl::Mat4 axisAngleRotationMatrix(const float& _angle, const ngl::Vec3& _axis) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Generate a random number
		/// This is used for stochastic L-systems during string rewriting.
		/// It is also used for space colonisation to generate random points.
		/// @return Random float in the range [0,1]
		//----------------------------------------------------------------------------------------------------------------------
		float generateRandomFloat() const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Count the number of branches in a string
		/// @param _string The string to count branches from
		/// @return The number of branches in the string
		//----------------------------------------------------------------------------------------------------------------------
		unsigned countBranches(const std::string& _string) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Add branches to the container
		/// @param _number The number of branches to add
		/// @param _position The position in the container to add at
		//----------------------------------------------------------------------------------------------------------------------
		void addBranches(const unsigned& _number, unsigned& _position);
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
		float calculateDecay(const unsigned& _depth) const;
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Calculate the branch positions using space colonisation
		/// @param _branch A reference to the branch to calculate
		/// @param _direction The direction to perform space colonisation in
		//----------------------------------------------------------------------------------------------------------------------
		void spaceColonisation(Branch& _branch, ngl::Vec3& _direction);
		//----------------------------------------------------------------------------------------------------------------------
		/// @brief Evaluate the L-system string to calculate branches
		/// This is calculated once per L-system update as it is an expensive operation
		//----------------------------------------------------------------------------------------------------------------------
		void evaluateBranches();

};

#endif // PLANT_H_
