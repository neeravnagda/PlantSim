#include <iostream>
#include "Plant.h"

//Initialise random device and number generator
std::random_device Plant::s_randomDevice;//Seed
std::mt19937 Plant::s_numberGenerator(Plant::s_randomDevice());//Mersienne Twister algorithm

Plant::Plant(std::string _blueprint)
{
		m_blueprint = PlantBlueprint::instance(_blueprint);
		m_string = *m_blueprint->getAxiom();
		stringToBranches();//Initialise the struct m_branches
}

Plant::~Plant()
{

}

void Plant::update()
{
		stringRewrite();
		stringToBranches();
		for (auto &b : m_branches)
		{
				std::cout<<"{"<<b.m_ID<<","<<b.m_creationDepth<<","<<b.m_string<<"} ";
		}
		std::cout<<"\n";
		branchesToString();
}

float Plant::genRand()
{
		std::uniform_real_distribution<float> distribute(0,1);
		return distribute(s_numberGenerator);
}

void Plant::stringRewrite()
{
		if (m_depth < *m_blueprint->getMaxDepth())//Only rewrite the string if the current depth is less than the max
		{
				std::string temp;//Temp string to store rewritten string

				bool isReplaced = false;//Check if a production rule was executed
				for (unsigned i=0, branchCount=0; i<m_string.length(); ++i)//Loop through each char in the string
				{
						isReplaced = false;
						for (ProductionRule r : *m_blueprint->getProductionRules())//Loop through the production rules
						{
								if(r.m_predecessor == m_string.substr(i,r.m_predecessor.length()))//Check if a substring matches the rule predecessor
								{
										if(r.m_probability > genRand())//For a stochastic L-system
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

void Plant::addBranches(unsigned _number, unsigned _position)
{
		for (unsigned i=0; i<_number; ++i, ++_position)
		{
				m_branches.emplace(m_branches.begin()+_position,
													 m_branches.size(),
													 m_depth+1,
													 std::string());
		}
}

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
						m_branches.emplace_back(m_branches.size(), m_depth, branchString);
				}

				branchStartPos = branchEndPos+1;
		}
}

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
