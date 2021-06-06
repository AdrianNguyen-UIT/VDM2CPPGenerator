#include "OperatorPrecedence.h"

namespace VDMParser
{
	OperatorPrecedence::OperatorPrecedence(const std::string& pName, const size_t& pLevel)
		: name(pName), level(pLevel)
	{
	}

	void OperatorPrecedence::SetName(const std::string& pName) 
	{ 
		name = pName; 
	}

	void OperatorPrecedence::SetLevel(const size_t& pLevel)
	{ 
		level = pLevel; 
	}

	const std::string& OperatorPrecedence::GetName() const 
	{ 
		return name; 
	}

	const size_t& OperatorPrecedence::GetLevel() const 
	{ 
		return level; 
	}
}