#include "Function.h"

namespace VDMParser
{
    Function::Function(const std::string& pName)
        : name(pName)
    {
    }

	void Function::Clear()
	{
		name.clear();
		params.clear();
		returnVar.Clear();
		statements.clear();
	}

	void Function::SetName(const std::string& pName)
	{ 
		name = pName;
	}

	void Function::SetReturnVarible(const Variable& pVar)
	{ 
		returnVar = pVar;
	}

	const std::string& Function::GetName() const
	{ 
		return name; 
	}

	const Variable& Function::GetReturnVariable() const
	{ 
		return returnVar; 
	}

	void Function::AddParameter(const Variable& param)
	{ 
		params.emplace_back(param); 
	}

	const std::vector<Variable>& Function::GetParameters() const
	{ 
		return params; 
	}

	const std::vector<Statement>& Function::GetStatements() const
	{
		return statements;
	}

	std::vector<Statement>& Function::GetStatements()
	{ 
		return statements; 
	}
}