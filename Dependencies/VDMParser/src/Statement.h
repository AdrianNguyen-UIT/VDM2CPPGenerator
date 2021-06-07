#pragma once
#include "DataType.h"
#include <vector>
namespace VDMParser
{
	enum class StatementType
	{
		SELECTION_STATEMENT,
		EXPRESSION_STATEMENT,
		EXPRESSION_LITERAL,
		EXPRESSION_VARIABLE_IDENTIFER,
		EXPRESSION_FUNCTION_CALL,
		EXPRESSION_OPERATOR_CALL,
		EXPRESSION_ASSIGNMENT_CALL,
		EXPRESSION_BOOL_IDENTIFER
	};

	class Statement
	{
	private:
		std::string name;
		DataType dataType;
		StatementType statementType;
		std::vector<Statement> compoundStatements;

	public:
		Statement(const std::string& pName = "", const DataType& pDataType = DataType(), const StatementType& pStatementType = StatementType::EXPRESSION_OPERATOR_CALL);
		void SetName(const std::string& pName);
		void SetDataType(const DataType& type);
		void SetStatementType(const StatementType& pStatementType);

		const std::string& GetName() const;
		std::string& GetName();
		const DataType& GetDataType() const;
		const std::vector<Statement>& GetCompoundStatements() const;
		std::vector<Statement>& GetCompoundStatements();
		const StatementType& GetStatementType() const;

		void AddInnerStatement(const Statement& statement);
	};
}