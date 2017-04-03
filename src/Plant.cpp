#include <cmath>
#include <iostream>
#include <stack>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/NGLStream.h>
#include "Plant.h"
//----------------------------------------------------------------------------------------------------------------------
//Initialise random device and number generator
std::random_device Plant::s_randomDevice;//Seed
std::mt19937 Plant::s_numberGenerator(Plant::s_randomDevice());//Mersienne Twister algorithm
//----------------------------------------------------------------------------------------------------------------------
Plant::Plant(std::string _blueprint, ngl::Vec3 _position)
{
	m_blueprint = PlantBlueprint::instance(_blueprint);
	m_string = m_blueprint->getAxiom();
	m_position = _position;
	stringToBranches();//Initialise the struct m_branches
	evaluateBranches();//Perform the first evaluation
}
//----------------------------------------------------------------------------------------------------------------------
Plant::~Plant(){}
//----------------------------------------------------------------------------------------------------------------------
void Plant::loadMatricesToShader(ngl::Mat4 _mouseGlobalTX, ngl::Mat4 _viewMatrix, ngl::Mat4 _projectionMatrix)
{
	ngl::ShaderLib *shader = ngl::ShaderLib::instance();
	ngl::Mat4 MV;
	ngl::Mat4 MVP;
	ngl::Mat3 N;
	ngl::Mat4 M;
	M = m_transform.getMatrix() * _mouseGlobalTX;
	MV = M * _viewMatrix;
	MVP = M * _projectionMatrix;
	N = MV;
	N.inverse();
	shader->setUniform("M", M);
	shader->setUniform("MV", MV);
	shader->setUniform("MVP", MVP);
	shader->setUniform("N", N);
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::draw(ngl::Mat4 _mouseGlobalTX, ngl::Mat4 _viewMatrix, ngl::Mat4 _projectionMatrix)
{
	ngl::VAOPrimitives *prim = ngl::VAOPrimitives::instance();

	unsigned lastBranchDepth = 0;
	float decay = 1.0f;
	std::stack<ngl::Vec3> positionStack;
	positionStack.emplace(m_position);
	std::stack<ngl::Vec3> rotationStack;
	rotationStack.emplace(ngl::Vec3(90,0,0));

	for (auto &b : m_branches)
	{
		//Pop transforms off the stacks if necessary
		for (unsigned i=b.m_creationDepth; i<=lastBranchDepth; ++i)
		{
			if (positionStack.size() > 1)
			{
				positionStack.pop();
			}
			if (rotationStack.size() > 1)
			{
				rotationStack.pop();
			}
		}
		//Recalculate the decay constant if the branch is at a different depth
		if (b.m_creationDepth != lastBranchDepth)
		{
			decay = calculateDecay(b.m_creationDepth);
		}

		//Set initial scale
		ngl::Vec3 branchScale = ngl::Vec3(m_blueprint->getRootRadius(), m_blueprint->getRootRadius(), m_blueprint->getDrawLength());
		branchScale *= decay;
		m_transform.setScale(branchScale);

		//Set the initial transformation
		m_transform.setPosition(positionStack.top() / 2);

		//Set the initial direction
		m_transform.setRotation(rotationStack.top());

		for (char &c : b.m_string)
		{
			switch (c)
			{
				case 'F' :
				{
					//Draw the cylinder
					m_transform.setScale(branchScale);
					loadMatricesToShader(_mouseGlobalTX, _viewMatrix, _projectionMatrix);
					prim->draw(m_blueprint->getGeometryName());

					//Update the position
					//										ngl::Vec3 newPos = m_blueprint->getDrawLength() * eulerToAxis(m_transform.getRotation()) * decay / 2;
					//										m_transform.addPosition(newPos);
					/*
										//Draw the sphere
										ngl::Vec3 sphereScale = ngl::Vec3(m_blueprint->getRootRadius(),m_blueprint->getRootRadius(),m_blueprint->getRootRadius());
										sphereScale *= decay;
										m_transform.setScale(sphereScale);
										loadMatricesToShader(_mouseGlobalTX, _viewMatrix, _projectionMatrix);
										prim->draw("sphere");
*/
					break;
				}
				case '/' :
				{
					m_transform.addRotation(0,0,m_blueprint->getDrawAngle());
					break;
				}
				case '\\' :
				{
					m_transform.addRotation(0,0,-(m_blueprint->getDrawAngle()));
					break;
				}
				case '+' :
				{
					m_transform.addRotation(m_blueprint->getDrawAngle(),0,0);
					break;
				}
				case '-' :
				{
					m_transform.addRotation(-(m_blueprint->getDrawAngle()),0,0);
					break;
				}
				case '&' :
				{
					m_transform.addRotation(0,m_blueprint->getDrawAngle(),0);
					break;
				}
				case '^' :
				{
					m_transform.addRotation(0,-(m_blueprint->getDrawAngle()),0);
					break;
				}
				default : break;
			}//End switch
		}//End for loop [char]

		//Update the variables
		positionStack.push(b.m_endPosition);
		rotationStack.push(m_transform.getRotation());
		lastBranchDepth = b.m_creationDepth;
	}//End for loop [branches]
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::update()
{
	stringRewrite();
	stringToBranches();
	evaluateBranches();
	branchesToString();
	/*
				for (auto &b : m_branches)
		{
				std::cout<<"{"<<b.m_ID<<","<<b.m_creationDepth<<","<<b.m_string<<",";
				std::cout<<"("<<b.m_endPosition.m_x<<","<<b.m_endPosition.m_y<<","<<b.m_endPosition.m_z<<")"<<"}";
		}
		std::cout<<"\n";
				*/
}
//----------------------------------------------------------------------------------------------------------------------
float Plant::genRand()
{
	std::uniform_real_distribution<float> distribute(0,1);
	return distribute(s_numberGenerator);
}
//----------------------------------------------------------------------------------------------------------------------
float Plant::calculateDecay(unsigned _depth)
{
	//Return 1 if the decay constant = 1. This avoids computing a pow
	if (m_blueprint->getDecayConstant() == 1.0f) return 1.0f;
	//Return decayConstant ^ _depth
	else return 1.0f / static_cast<float>(pow(m_blueprint->getDecayConstant(), _depth));
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::stringRewrite()
{
	if (m_depth < m_blueprint->getMaxDepth())//Only rewrite the string if the current depth is less than the max
	{
		std::string temp;//Temp string to store rewritten string

		bool isReplaced = false;//Check if a production rule was executed
		for (unsigned i=0, branchCount=0; i<m_string.length(); ++i)//Loop through each char in the string
		{
			isReplaced = false;
			for (ProductionRule r : m_blueprint->getProductionRules())//Loop through the production rules
			{
				if(r.m_predecessor == m_string.substr(i,r.m_predecessor.length()))//Check if a substring matches the rule predecessor
				{
					//Only calculate a random number if the probability is not 1
					if( (r.m_probability == 1.0f) || (r.m_probability > genRand()) )
					{
						temp += r.m_successor;//Add the replaced rule
						i += r.m_predecessor.length() - 1;//Iterate further through the original string if predecessor length > 1 char
						isReplaced = true;
						addBranches(countBranches(r.m_successor), branchCount);//Add to the container m_branches
					}
					break;//Avoid checking other rules
				}
			}
			if (isReplaced == false) { temp += m_string[i]; }//No replacement was made, so add the original char
			branchCount = countBranches(temp);
		}
		m_string = temp;//Assign the temp string to the member variable
		++m_depth;//Increment the depth of the expansion
	}
}
//----------------------------------------------------------------------------------------------------------------------
//Count the number of '[' in a string
unsigned Plant::countBranches(std::string _string)
{
	int branchCount = 0;
	for (char c : _string)
	{
		if (c == '[') ++branchCount;
	}
	return branchCount;
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::addBranches(unsigned _number, unsigned _position)
{
	for (unsigned i=0; i<_number; ++i, ++_position)
	{
		m_branches.emplace(m_branches.begin()+_position,
											 m_branches.size(),
											 m_depth+1);
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::stringToBranches()
{
	unsigned numBranches = countBranches(m_string);

	std::size_t branchStartPos=0;
	std::size_t branchEndPos=0;
	for (unsigned i=0; i<numBranches; ++i)
	{
		//Find start and end position in a string of the branches
		branchStartPos = m_string.find_first_not_of("[]", branchStartPos);
		branchEndPos = m_string.find_first_of("[]", branchStartPos+1);
		std::string branchString = m_string.substr(branchStartPos, branchEndPos - branchStartPos);

		if (i < m_branches.size())
		{
			m_branches[i].m_string = branchString;
		}
		else
		{
			m_branches.emplace_back(m_branches.size(), m_depth, ngl::Vec3::zero(), branchString);
		}

		branchStartPos = branchEndPos+1;
	}
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::branchesToString()
{
	std::string temp;
	//Add all but the last branch
	for (unsigned i=0; i<m_branches.size()-1; ++i)
	{
		temp += "[" + m_branches[i].m_string;//Start a branch and add its string
		//If the next branch was created at the same depth or earlier, end this branch
		for (int j=0; j<=static_cast<int>(m_branches[i].m_creationDepth - m_branches[i+1].m_creationDepth); ++j)
		{
			temp += "]";
		}
	}
	//Add the last branch as the above would require accessing an element out of bounds
	temp += "[" + m_branches.back().m_string;
	for (int j=0; j<=static_cast<int>(m_branches.back().m_creationDepth); ++j)
	{
		temp += "]";
	}
	m_string = temp;
}
//----------------------------------------------------------------------------------------------------------------------
void Plant::evaluateBranches()
{
	//Precompute sin and cos functions for 2d rotation matrices
	const float cosTheta = cos(m_blueprint->getDrawAngleRadians());
	const float sinTheta = sin(m_blueprint->getDrawAngleRadians());

	const float drawLength = m_blueprint->getDrawLength();
	unsigned lastBranchDepth = 0;//This is needed as a range based for loop is being used
	float decay = 1;

	//Make a transformation stack for positions and direction vectors
	std::stack<ngl::Vec3> positionStack;
	positionStack.emplace(m_position);//Initialise with the root position
	std::stack<ngl::Vec3> directionStack;
	directionStack.emplace(ngl::Vec3(0.0f, m_blueprint->getDrawLength(), 0.0f));//Initialise to go up

	for (auto &b : m_branches)
	{
		for (unsigned i=b.m_creationDepth; i<=lastBranchDepth; ++i)
		{
			if (positionStack.size() > 1)
			{
				positionStack.pop();
			}
			if (positionStack.size() > 1)
			{
				directionStack.pop();
			}
		}

		//Recalculate decay constant if necessary
		if (b.m_creationDepth != lastBranchDepth)
		{
			decay = calculateDecay(b.m_creationDepth);
		}

		//If the branch end position has already been calculated, don't recalculate it
		if (b.m_endPosition != ngl::Vec3::zero())
		{
			ngl::Vec3 rot = b.m_endPosition - positionStack.top();
			rot.normalize();
			directionStack.push(rot);
			positionStack.push(b.m_endPosition);
		}
		//This calculates the end position of a branch
		else
		{
			ngl::Vec3 currentPos = positionStack.top();
			ngl::Vec3 currentRot = directionStack.top();
			//Evaluate each character to update position and directions
			for (char c : b.m_string)
			{
				switch (c)
				{
					//calculate a new position in the forward direction
					//halve the draw length for each iteration in the L-system
					case static_cast<char>('F') :
					{
						currentPos += drawLength * currentRot * decay;
						break;
					}
						//rotate the direction vector by +theta on the xy plane
					case static_cast<char>('/') :
					{
						float x = cosTheta*currentRot.m_x - sinTheta*currentRot.m_y;
						float y = sinTheta*currentRot.m_x + cosTheta*currentRot.m_y;
						currentRot.m_x = x;
						currentRot.m_y = y;
						currentRot.normalize();
						break;
					}
						//rotate the direction vector by -theta on the xy plane
					case static_cast<char>('\\') :
					{
						float x = cosTheta*currentRot.m_x + sinTheta*currentRot.m_y;
						float y = cosTheta*currentRot.m_y - sinTheta*currentRot.m_x;
						currentRot.m_x = x;
						currentRot.m_y = y;
						currentRot.normalize();
						break;
					}
						//rotate the direction vector by +theta on the yz plane
					case static_cast<char>('+') :
					{
						float y = cosTheta*currentRot.m_y - sinTheta*currentRot.m_z;
						float z = sinTheta*currentRot.m_y + cosTheta*currentRot.m_z;
						currentRot.m_y = y;
						currentRot.m_z = z;
						currentRot.normalize();
						break;
					}
						//rotate the direction vector by -theta on the yz plane
					case static_cast<char>('-') :
					{
						float y = cosTheta*currentRot.m_y + sinTheta*currentRot.m_z;
						float z = cosTheta*currentRot.m_z - sinTheta*currentRot.m_y;
						currentRot.m_y = y;
						currentRot.m_z = z;
						currentRot.normalize();
						break;
					}
						//rotate the direction vector by +theta on the xz plane
					case static_cast<char>('&') :
					{
						float x = cosTheta*currentRot.m_x + sinTheta*currentRot.m_z;
						float z = cosTheta*currentRot.m_z - sinTheta*currentRot.m_x;
						currentRot.m_x = x;
						currentRot.m_z = z;
						currentRot.normalize();
						break;
					}
						//rotate the direction vector by -theta on the xz plane
					case static_cast<char>('^') :
					{
						float x = cosTheta*currentRot.m_x - sinTheta*currentRot.m_z;
						float z = cosTheta*currentRot.m_z - sinTheta*currentRot.m_x;
						currentRot.m_x = x;
						currentRot.m_z = z;
						currentRot.normalize();
						break;
					}
					default:
						break;
				}//End switch statement
			}//End for loop [chars]

			//Check for collisions in R-tree
			//Update string
			//Recompute values
			//Add to R-tree

			//Push new values to the stacks
			positionStack.push(currentPos);
			directionStack.push(currentRot);
			b.m_endPosition = currentPos;//Update the value in the branch struct
		}//End else condition
		lastBranchDepth = b.m_creationDepth;
	}//End for loop [branches]
}//End function
//----------------------------------------------------------------------------------------------------------------------
