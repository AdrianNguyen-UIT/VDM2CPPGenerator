#include "Statement.h"

namespace VDMParser
{
    Statement::Statement(const std::string& pName, const DataType& pDataType, const StatementType& pStatementType)
        : name(pName), dataType(pDataType), statementType(pStatementType)
    {
    }

	void Statement::SetName(const std::string& pName) 
	{ 
		name = pName; 
	}

	void Statement::SetDataType(const DataType& type) 
	{ 
		dataType = type; 
	}

	void Statement::SetStatementType(const StatementType& pStatementType)
	{ 
		statementType = pStatementType; 
	}

	const std::string& Statement::GetName() const
	{ 
		return name; 
	}

	std::string& Statement::GetName()
	{ 
		return name; 
	}

	const DataType& Statement::GetDataType() const
	{ 
		return dataType; 
	}

	const std::vector<Statement>& Statement::GetCompoundStatements() const 
	{ 
		return compoundStatements; 
	}

	std::vector<Statement>& Statement::GetCompoundStatements()
	{ 
		return compoundStatements; 
	}

	const StatementType& Statement::GetStatementType() const 
	{ 
		return statementType; 
	}

	void Statement::Statement::AddInnerStatement(const Statement& statement)
	{ 
		compoundStatements.push_back(statement); 
	}
}