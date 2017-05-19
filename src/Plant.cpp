#include <cmath>
#include <stack>
#include <ngl/Mat3.h>
#include <ngl/Vec4.h>
#include <ngl/ShaderLib.h>
#include <ngl/NGLStream.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Util.h>
#include "Plant.h"
//----------------------------------------------------------------------------------------------------------------------
//Define static members
std::random_device Plant::s_randomDevice;
std::mt19937 Plant::s_numberGenerator(Plant::s_randomDevice());
//----------------------------------------------------------------------------------------------------------------------
Plant::Plant(const std::string& _blueprint, const ngl::Vec3& _position)
{
	//Initialise the object
	m_blueprint = PlantBlueprint::instance(_blueprint);
	m_string = m_blueprint->axiom();
	m_position = _position;

	//Initialise the simulation
	stringToBranches();
	evaluateBranches();
}
//----------------------------------------------------------------------------------------------------------------------
Plant::~Plant(){}
//----------------------------------------------------------------------------------------------------------------------
void Plant::loadMatricesToShader(const ngl::Mat4& _mouseGlobalTX, const ngl::Mat4 _viewMatrix, const ngl::Mat4 _projectionMatrix) const
{
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	//Create the matrices
	ngl::Mat4 M = m_transform * _mouseGlobalTX;
	ngl::Mat4 MV = M * _viewMatrix;
	ngl::Mat4 MVP = MV * _projectionMatrix;
	ngl::Mat3 N = MV;
	N.inverse();
	//Set the uniforms in the shader
	shader->setUniform("M", M);
	shader->setUniform("MV", MV);
	shader->setUniform("MVP", MVP);
	shader->setUniform("N", N);
	//The texture does not need to tile
	shader->setUniform("texScale", 1.0f);
}
//----------------------------------------------------------------------------------------------------------------------
//Rotation matrix found from https://en.wikipedia.org/wiki/Rotation_matrix
ngl::Mat4 Plant::axisAngleRotationMatrix(const float& _angle, const ngl::Vec3& _axis) const
{
	//Precompute trig functions
	const float cosTheta = cos(_angle);
	const float oneMinusCosTheta = 1 - cosTheta;
	const float sinTheta = sin(_angle);

	//Generate the rotation matrix
	ngl::Mat4 rot;
	rot.m_00 = cosTheta + _axis.m_x * _axis.m_x * oneMinusCosTheta;
	rot.m_01 = _axis.m_y * _axis.m_x * oneMinusCosTheta + _axis.m_z * sinTheta;
	rot.m_02 = _axis.m_z * _axis.m_x * oneMinusCosTheta - _axis.m_y * sinTheta;
	rot.m_03 = 0.0f;

	rot.m_10 = _axis.m_x * _axis.m_y * oneMinusCosTheta - _axis.m_z * sinTheta;
	rot.m_11 = cosTheta + _axis.m_y * _axis.m_y * oneMinusCosTheta;
	rot.m_12 = _axis.m_z * _axis.m_y * oneMinusCosTheta + _axis.m_x * sinTheta;
	rot.m_13 = 0.0f;

	rot.m_20 = _axis.m_x * _axis.m_z * oneMinusCosTheta + _axis.m_y * sinTheta;
	rot.m_21 = _axis.m_y * _axis.m_z * oneMinusCosTheta - _axis.m_x * sinTheta;
	rot.m_22 = cosTheta + _axis.m_z * _axis.m_z * oneMinusCosTheta;
	rot.m_23 = 0.0f;

	rot.m_30 = 0.0f;
	rot.m_31 = 0.0f;
	rot.m_32 = 0.0f;
	rot.m_33 = 1.0f;

	return rot;
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::draw(const ngl::Mat4& _mouseGlobalTX, const ngl::Mat4 _viewMatrix, const ngl::Mat4 _projectionMatrix)
{
	//Set some initial parameters
	float decay = 1.0f;
	ngl::Vec3 initialDirection = ngl::Vec3(0.0f,1.0f,0.0f);

	//Draw each branch
	for (Branch &b : m_branches)
	{
		//Pre-compute the decay for the branch
		decay = calculateDecay(b.m_creationDepth);

		//Draw the branch nodes
		for (unsigned i=1; i<b.m_nodePositions.size(); ++i)
		{
			//Calculate the direction and length of the segment
			ngl::Vec3 dir = b.m_nodePositions[i] - b.m_nodePositions[i-1];
			float length = dir.length();
			dir.normalize();

			//Calculate the scale matrix
			ngl::Mat4 scaleMatrix;
			scaleMatrix.scale(m_blueprint->rootRadius()*decay, length, m_blueprint->rootRadius()*decay);

			//Calculate the axis and angle for the rotation matrix
			float angle = acos(initialDirection.dot(dir));
			ngl::Mat4 rotationMatrix;
			if (angle > 0)
			{
				ngl::Vec3 axis;
				axis.cross(initialDirection, dir);
				axis.normalize();
				rotationMatrix = axisAngleRotationMatrix(angle, axis);
			}

			//Update the position
			ngl::Vec3 position = b.m_nodePositions[i-1] + dir*length/2;
			m_transform =  scaleMatrix * rotationMatrix;
			m_transform.m_30 = position.m_x;
			m_transform.m_31 = position.m_y;
			m_transform.m_32 = position.m_z;

			//Load the matrices to the shader and draw the branch part
			loadMatricesToShader(_mouseGlobalTX, _viewMatrix, _projectionMatrix);
			PlantBlueprint::drawCylinder();
		}

		glDisable(GL_CULL_FACE);	//Disable face culling for the leaves
		//Draw the leaves
		ngl::Vec3 leafInitialDirection = ngl::Vec3(1.0f, 0.0f, 0.0f);
		for (unsigned i=0; i<b.m_leafPositions.size(); ++i)
		{
			//Calculate the scale matrix
			ngl::Mat4 scaleMatrix;
			scaleMatrix.scale(m_blueprint->leafScale(), m_blueprint->leafScale(), m_blueprint->leafScale());

			//Calculate the rotation matrix
			float angle = acos(leafInitialDirection.dot(b.m_leafOrientations[i]));
			ngl::Mat4 rotationMatrix;
			if (angle > 0)	//The rotation matrix is undefined if the angle is 0
			{
				ngl::Vec3 axis;
				axis.cross(leafInitialDirection, b.m_leafOrientations[i]);
				axis.normalize();
				rotationMatrix = axisAngleRotationMatrix(angle, axis);
			}

			//Calculate the model matrix
			m_transform = scaleMatrix * rotationMatrix;
			m_transform.m_30 = b.m_leafPositions[i].m_x;
			m_transform.m_31 = b.m_leafPositions[i].m_y;
			m_transform.m_32 = b.m_leafPositions[i].m_z;

			//Load the matrices to the shader and draw the leaf
			loadMatricesToShader(_mouseGlobalTX, _viewMatrix, _projectionMatrix);
			PlantBlueprint::drawLeaf();
		}
		glEnable(GL_CULL_FACE);	//Re-enable face culling for the branches
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::updateSimulation()
{
	//Only update if the current depth is less than the max
	if (m_depth < m_blueprint->maxDepth())
	{
		++m_depth;//Increment the depth of the expansion
		stringRewrite();
		evaluateBranches();
	}
}
//----------------------------------------------------------------------------------------------------------------------
float Plant::generateRandomFloat() const
{
	std::uniform_real_distribution<float> distribute(0,1);
	return distribute(s_numberGenerator);
}
//----------------------------------------------------------------------------------------------------------------------
unsigned Plant::countCharInString(const std::string& _string, const char& _c) const
{
	unsigned count = 0;
	for (char c: _string)
	{
		if (c == _c) ++count;
	}
	return count;
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::addBranches(const unsigned& _number, unsigned& _position)
{
	for (unsigned i=0; i<_number; ++i, ++_position)
	{
		m_branches.emplace(m_branches.begin()+_position, m_depth);
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::stringToBranches()
{
	unsigned numBranches = countCharInString(m_string, '[');

	//Initialise the start and end positions of branches in the string
	std::size_t branchStartPos = 0;
	std::size_t branchEndPos = 0;

	for (unsigned i=0; i<numBranches; ++i)
	{
		//The branch starts at the first character that's not a bracket
		branchStartPos = m_string.find_first_not_of("[]", branchStartPos);
		//The branch ends at the next bracket - open or closed
		branchEndPos = m_string.find_first_of("[]", branchStartPos+1);

		//Find a substring
		std::string branchString = m_string.substr(branchStartPos, branchEndPos - branchStartPos);

		//If the branch already exists, update the string
		if (i<m_branches.size())
		{
			m_branches[i].m_string = branchString;
		}
		//Otherwise create a new branch with the string
		else
		{
			m_branches.emplace_back(m_depth, branchString);
		}
		branchStartPos = branchEndPos+1;	//Increment the start position to check the next branch
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::stringRewrite()
{
	//Count the number of draw calls, will rerun this function if the count is the same
	unsigned fCount = countCharInString(m_string, 'F');

	std::string newString;	//Temporary new string to add to
	bool isReplaced = false;//Check if a production rule was executed

	for (unsigned i=0, branchCount=0; i<m_string.length(); ++i)//Loop through each char in the string
	{
		isReplaced = false;
		for (ProductionRule r : m_blueprint->productionRules())//Loop through the production rules
		{
			if(r.m_predecessor == m_string.substr(i,r.m_predecessor.length()))//Check if a substring matches the rule predecessor
			{
				newString += r.m_successor;//Add the replaced rule
				i += r.m_predecessor.length() - 1;//Iterate further through the original string if predecessor length > 1 char
				isReplaced = true;
				//Add new branches to the container
				addBranches(countCharInString(r.m_successor, '['), branchCount);
				break;//Avoid checking other rules
			}
		}
		if (isReplaced == false) { newString += m_string[i]; }//No replacement was made, so add the original char
		branchCount = countCharInString(newString, '[');
	}
	m_string = newString;//Assign the temp string to the member variable

	//Update the string in each branch
	stringToBranches();

	//Check if the drawing will be the same, i.e. if this function needs to be rerun
	if (countCharInString(m_string, 'F') == fCount)
	{
		stringRewrite();
	}
}
//----------------------------------------------------------------------------------------------------------------------
float Plant::calculateDecay(const unsigned& _depth) const
{
	//Return 1 if the decay constant = 1. This avoids computing a pow
	if (m_blueprint->decayConstant() == 1.0f) return 1.0f;
	//Return decayConstant ^ _depth
	else return 1.0f / static_cast<float>(pow(m_blueprint->decayConstant(), _depth));
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::scatterLeaves(Branch& _branch, const ngl::Vec3 &_startPos, const ngl::Vec3 &_endPos, const float _radius, const ngl::Vec3& _direction)
{
	unsigned count = m_blueprint->leavesPerBranch() / m_blueprint->controlPointsPerBranch();//The number of leaves per node
	float segmentLength = (_startPos - _endPos).length();//The length of the branch segment required for the unoriented cylinder height

	//Calculate a rotation matrix to rotate a cylinder pointing up to the input direction
	ngl::Vec3 axis;
	axis.cross(_direction, ngl::Vec3::up());
	float angle = acos(_direction.dot(ngl::Vec3::up()));
	ngl::Mat4 rotationMatrix = axisAngleRotationMatrix(angle, axis);

	//Compute each leaf
	for (unsigned i=0; i<count; ++i)
	{
		//Calculate a random point and its normal on the surface of a cylinder pointing up
		float height = generateRandomFloat() * segmentLength;
		float theta = generateRandomFloat() * ngl::TWO_PI;
		ngl::Vec3 unorientedPosition(_radius * cos(theta), height, _radius * sin(theta));
		ngl::Vec3 unorientedNormal(unorientedPosition.m_x, 0.0f, unorientedPosition.m_z);
		unorientedNormal.normalize();

		//Rotate the point and normal
		ngl::Vec4 position = rotationMatrix * ngl::Vec4(unorientedPosition);

		ngl::Vec4 normal = rotationMatrix * ngl::Vec4(unorientedNormal);
		normal.normalize();

		//Add the original position and the normal
		position += ngl::Vec4(_startPos) + normal * m_blueprint->leafScale() / 2;

		//Add a random rotation to the normal - this is a maximum of +- 15 degrees in each axis
		normal.m_x += (generateRandomFloat() - 0.5f) * ngl::PI / 6;
		normal.m_y += (generateRandomFloat() - 0.5f) * ngl::PI / 6;
		normal.m_z += (generateRandomFloat() - 0.5f) * ngl::PI / 6;

		normal.normalize();	//Need to renormalise to account for the added random

		//Store the position and orientation in the vectors in the branch
		_branch.m_leafPositions.emplace_back(position.toVec3());
		_branch.m_leafOrientations.emplace_back(normal.toVec3());
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::spaceColonisation(Branch& _branch, ngl::Vec3& _direction)
{
	float decay = calculateDecay(_branch.m_creationDepth);	//Precompute the decay of the branch
	float maxLength = m_blueprint->drawLength() * decay;//Max length of the branch bit
	ngl::Vec3 pos = _branch.m_nodePositions.back();//Initialise position for this branch bit

	//Create some segments
	for (unsigned i=1; i<m_blueprint->controlPointsPerBranch(); ++i)
	{
		ngl::Vec3 startPos = pos;	//Temp variable for scatterLeavesFunction

		//Predefine variables
		float h, r, alpha;

		//Generate a random length, radius and angle to create a random point inside a cone
		if (m_blueprint->controlPointsPerBranch() > 2)
		{
			h = generateRandomFloat();
			r = generateRandomFloat() * h * m_blueprint->maxDeviation();
			alpha = generateRandomFloat() * ngl::TWO_PI;
			h *= maxLength / m_blueprint->controlPointsPerBranch();
		}
		//Don't generata random values for height as this is a rigid L-system
		else
		{
			h = maxLength;
			//If the max deviation > 0, compute a random deviation, otherwise set to 0
			r = (m_blueprint->maxDeviation() > 0) ? generateRandomFloat() * h * m_blueprint->maxDeviation() : 0.0f;
			alpha = 0.0f;
		}

		//Convert the cylindrical coordinates to cartesian coordinates
		ngl::Vec3 randPoint;
		randPoint.m_x = r * cos(alpha);
		randPoint.m_y = h;
		randPoint.m_z = r * sin(alpha);

		//Calculate a rotation matrix for the position
		float angle = acos(_direction.dot(ngl::Vec3::up()));
		ngl::Vec3 axis;
		axis.cross(_direction, ngl::Vec3::up());
		ngl::Mat4 rotationMatrix = axisAngleRotationMatrix(angle, axis);

		//Calculate the new position and direction
		ngl::Vec4 newPos = rotationMatrix * ngl::Vec4(randPoint, 1.0f);
		newPos += ngl::Vec4(pos);
		//Make sure the branch doesn't go below ground
		if (newPos.m_y <= 0.0f) newPos.m_y *=-1;
		_direction = newPos.toVec3() - pos;
		float nodeLength = _direction.length();

		//Add phototrophism if applicable
		if (m_blueprint->phototropismScaleFactor() > 0)
		{
			ngl::Vec3 phototropism = PlantBlueprint::sunPosition() - pos;//Calculate the direction to the sun
			phototropism.normalize();
			phototropism *= m_blueprint->phototropismScaleFactor() * decay;
			_direction += phototropism;
		}
		//Add gravitropism if applicable
		if (m_blueprint->gravitropismScaleFactor() > 0)
		{
			ngl::Vec3 gravitropism = ngl::Vec3::down() * m_blueprint->gravitropismScaleFactor() * decay;
			_direction += gravitropism;
		}
		//Calculate the new position
		_direction.normalize();
		pos += _direction * nodeLength;

		//Add the position to the end of the array
		_branch.m_nodePositions.emplace_back(pos);

		//Calculate leaves
		if (_branch.m_creationDepth >= m_blueprint->leavesStartDepth())
		{
			scatterLeaves(_branch ,startPos, pos, decay * m_blueprint->rootRadius(), _direction);
		}
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::evaluateBranches()
{
	//Create a stack of positions used to start new branches
	std::stack<ngl::Vec3> positionStack;
	positionStack.emplace(m_position);//Initialise with plant position
	//Create a stack of directions for branch starting directions
	std::stack<ngl::Vec3> directionStack;
	directionStack.emplace(ngl::Vec3::up());//Initialise with the up vector

	unsigned lastBranchDepth = 0;
	for (auto &b : m_branches)
	{
		//Pop values off the stack
		for (unsigned i=b.m_creationDepth; i<=lastBranchDepth; ++i)
		{
			if (positionStack.size() > 1) positionStack.pop();
			if (directionStack.size() > 1) directionStack.pop();
		}

		//Add the last position to the position stack
		//This is to avoid resimulating the branch
		if (!b.m_nodePositions.empty())
		{
			ngl::Vec3 direction = b.m_nodePositions.back() - b.m_nodePositions.front();
			directionStack.push(direction);
			positionStack.push(b.m_nodePositions.back());
		}
		//Perform the space colonisation algorithm
		else
		{
			b.m_nodePositions.emplace_back(positionStack.top());//Initialise the start position of the branch
			ngl::Vec3 direction = directionStack.top();//Create a temporary variable for the direction of the branch nodes

			// evaluate the string to find the new direction
			for (char c : b.m_string)
			{
				switch (c)
				{
					//Rotate by +theta on the xy plane
					case '/' :
					{
						ngl::Mat3 r;
						r.rotateZ(m_blueprint->drawAngle());
						direction = r * direction;
						break;
					}
					//Rotate by -theta on the xy plane
					case '\\' :
					{
						ngl::Mat3 r;
						r.rotateZ(-m_blueprint->drawAngle());
						direction = r * direction;
						break;
					}
					//Rotate by +theta on the yz plane
					case '+' :
					{
						ngl::Mat3 r;
						r.rotateX(m_blueprint->drawAngle());
						direction = r * direction;
						break;
					}
					//Rotate by -theta on the yz plane
					case '-' :
					{
						ngl::Mat3 r;
						r.rotateX(-m_blueprint->drawAngle());
						direction = r * direction;
						break;
					}
					//Rotate by +theta on the xz plane
					case '&' :
					{
						ngl::Mat3 r;
						r.rotateY(m_blueprint->drawAngle());
						direction = r * direction;
						break;
					}
					//Rotate by -theta on the xz plane
					case '^' :
					{
						ngl::Mat3 r;
						r.rotateY(-m_blueprint->drawAngle());
						direction = r * direction;
						break;
					}
					//Do some space colonisation in the current direction
					case 'F' :
					{
						direction.normalize();
						spaceColonisation(b, direction);
						break;
					}
					default : break;
				}//End switch
			}//End for [char]

			// update the stacks
			positionStack.push(b.m_nodePositions.back());
			directionStack.push(b.m_nodePositions.back() - b.m_nodePositions.front());

		}//End else condition
		lastBranchDepth = b.m_creationDepth;
	}//End for [branches]
}
//----------------------------------------------------------------------------------------------------------------------
