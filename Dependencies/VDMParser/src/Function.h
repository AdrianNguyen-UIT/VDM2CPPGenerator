#pragma once
#include "Variable.h"
#include "Statement.h"
namespace VDMParser
{
	class Function
	{
	private:
		std::string name;
		std::vector<Variable> params;
		Variable returnVar;
		std::vector<Statement> statements;

	public:
		Function(const std::string &pName = "");
		void Clear();

		void SetName(const std::string& pName);
		void SetReturnVarible(const Variable& pVar);

		const std::string& GetName() const;
		const Variable& GetReturnVariable() const;

		void AddParameter(const Variable& param);
		const std::vector<Variable>& GetParameters() const;
		const std::vector<Statement>& GetStatements() const;
		std::vector<Statement>& GetStatements();

	};
}