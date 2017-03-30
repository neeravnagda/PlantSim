#ifndef PRODUCTIONRULE_H_
#define PRODUCTIONRULE_H_

#include <string>

/// @file ProductionRule.h
/// @brief this struct allows the L-system production rules to be put in a custom container
/// @author Neerav Nagda
/// @version 1.0
/// @date 18/03/17
/// @struct ProductionRule
/// @brief struct to contain information about the L-system production rules

typedef struct ProductionRule
{
		std::string m_predecessor;	// Character(s) to be replaced
		std::string m_successor;	// Character(s) to replace
		float m_probability;	// Probability of replacing predecessor with successor
		ProductionRule(std::string _predecessor, std::string _successor, float _probability) ://Constructor
			m_predecessor(_predecessor),
			m_successor(_successor),
			m_probability(_probability){}
} ProductionRule;

#endif // PRODUCTIONRULE_H_
