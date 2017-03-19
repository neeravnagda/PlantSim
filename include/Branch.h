#ifndef BRANCH_H_
#define BRANCH_H_

#include <string>

/// @file Branch.h
/// @brief this struct is used to contain substrings from the L-system string
/// @author Neerav Nagda
/// @version 1.2
/// @date 19/03/17
/// @struct Branch
/// @brief struct to contain branch information from L-system string

typedef struct Branch
{
		unsigned m_ID;//ID of the branch
		unsigned m_creationDepth;//Depth of tree when branch was created
		std::string m_string;//L-system string of the branch
		Branch(int _ID, int _depth, std::string _string) ://Constructor
				m_ID(_ID),
				m_creationDepth(_depth),
				m_string(_string){}
} Branch;

#endif // BRANCH_H_
