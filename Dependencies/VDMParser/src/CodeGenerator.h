#pragma once
#include "Function.h"
#include <stdexcept>
namespace VDMParser
{
	class CodeGenerator
	{
	private:

		std::vector<DataType> dataTypes;
		Function function;
		std::vector<std::string> headerIncludes;

	public:
		CodeGenerator();

		std::string Generate(const Function& pFunction);
		std::string GenerateTemplate();

	private:
		void InitDataTypes();
		std::string ParseVariableDataTypeName(const DataType& type);
		std::string ParseParameterList(const std::vector<Variable>& params, bool isRef = false);
		std::string ParseFunctionDeclerationStatement(const DataType& returnDatatype, const std::string &functionName, const std::vector<Variable> &params, bool isRef = false);
		std::string ParseTargetFunctionDefinition();
		std::string ParseVariableDeclerationStatement(const Variable &var);
		std::string ParseInputFuntionDefinition(const std::string& functionName, const std::vector<Variable>& params);
		std::string ParseOutputFuntionDefinition(const std::string& functionName, const std::vector<Variable>& params);
		std::string ParseMainFuntionDefinition();

		//Utilites
		std::string NewLine();
		std::string BeginBlock();
		std::string CloseBlock();
		std::string Indent(size_t size = 1);
		std::string GetInput(const Variable& var);
		std::string PrintOutput(const Variable& var);
		void AddHeaderInclude(const std::string &headerName);

		std::string ParseStatement(const Statement& statement, size_t indent = 1);
		std::string ParseIfStatement(const Statement& statement, size_t indent = 1);
		std::string ParseAssignmentStatement(const Statement& statement, size_t indent = 1);
		std::string ParseOperatorCallStatement(const Statement& statement);
		std::string ParseReturnStatement(const Variable& var, size_t indent = 1);
		std::string ParseReturnStatement(const int code, size_t indent = 1);
		std::string ParseFunctionCallStatement(const std::string& functionName, const std::vector<Variable>& params, size_t indent = 1);
		std::string ParseHeaderInclude();
	};
}