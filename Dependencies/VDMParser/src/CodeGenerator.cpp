#include "CodeGenerator.h"

namespace VDMParser
{
	CodeGenerator::CodeGenerator()
	{
		InitDataTypes();

	}

	std::string CodeGenerator::Generate(const Function& pFunction)
	{
		function = pFunction;

		std::string result;

		AddHeaderInclude("iostream");

		std::vector<Variable> vars = function.GetParameters();
		vars.emplace_back(function.GetReturnVariable());
		for (Variable var : vars)
		{
			if (var.GetDataType().GetType() == Type::STRING)
				AddHeaderInclude("string");
		}

		result += ParseHeaderInclude();
		result += NewLine();

		result += ParseFunctionDeclerationStatement(DataType("", Type::VOID), "GetInput", function.GetParameters(), true);
		std::vector<Variable> params = { function.GetReturnVariable() };
		result += ParseFunctionDeclerationStatement(DataType("", Type::VOID), "PrintOutput", params);
		result += ParseFunctionDeclerationStatement(function.GetReturnVariable().GetDataType(), function.GetName(), function.GetParameters());
		result += NewLine();

		result += ParseMainFuntionDefinition();
		result += NewLine();
		result += ParseInputFuntionDefinition("GetInput", function.GetParameters());
		result += NewLine();
		result += ParseOutputFuntionDefinition("PrintOutput", params);
		result += NewLine();
		result += ParseTargetFunctionDefinition();
		return result;
	}

	std::string CodeGenerator::GenerateTemplate()
	{
		std::string result;
		result += "int main()";
		result += NewLine();
		result += BeginBlock();
		result += ParseReturnStatement(0);
		result += CloseBlock();
		return result;
	}

	void CodeGenerator::InitDataTypes()
	{
		dataTypes.reserve(6);
		DataType type;
		type.SetName("int");
		type.SetType(Type::INT);
		dataTypes.push_back(type);

		type.SetName("unsigned int");
		type.SetType(Type::UINT);
		dataTypes.push_back(type);

		type.SetName("float");
		type.SetType(Type::DOUBLE);
		dataTypes.push_back(type);

		type.SetName("bool");
		type.SetType(Type::BOOL);
		dataTypes.push_back(type);

		type.SetName("std::string");
		type.SetType(Type::STRING);
		dataTypes.push_back(type);

		type.SetName("void");
		type.SetType(Type::VOID);
		dataTypes.push_back(type);
	}

	std::string CodeGenerator::ParseVariableDataTypeName(const DataType& type)
	{
		for (DataType dataType : dataTypes)
		{
			if (dataType.GetType() == type.GetType())
			{
				return dataType.GetName();
			}
		}

		throw std::runtime_error("Data type is not found!");
	}

	std::string CodeGenerator::ParseParameterList(const std::vector<Variable>& params, bool isRef)
	{
		std::string result;
		result += "(";
		for (size_t index = 0; index < params.size(); ++index)
		{
			if (isRef)
			{
				result += ParseVariableDataTypeName(params[index].GetDataType()) + "& " + params[index].GetName();
			}
			else
			{
				result += ParseVariableDataTypeName(params[index].GetDataType()) + " " + params[index].GetName();
			}

			if (index != params.size() - 1)
			{
				result += ", ";
			}
		}
		result += ")";
		return result;
	}

	std::string CodeGenerator::ParseFunctionDeclerationStatement(const DataType& returnDatatype, const std::string& functionName, const std::vector<Variable>& params, bool isRef)
	{
		std::string result;
		result += ParseVariableDataTypeName(returnDatatype) + " " + functionName;
		result += ParseParameterList(params, isRef);
		result += ";";
		result += NewLine();
		return result;
	}

	std::string CodeGenerator::ParseTargetFunctionDefinition()
	{
		std::string result;
		result += ParseVariableDataTypeName(function.GetReturnVariable().GetDataType()) + " " + function.GetName();
		result += ParseParameterList(function.GetParameters());
		result += NewLine();
		result += BeginBlock();
		result += ParseVariableDeclerationStatement(function.GetReturnVariable());

		for (Statement statement : function.GetStatements())
		{
			result += ParseStatement(statement);
		}

		result += ParseReturnStatement(function.GetReturnVariable());
		result += CloseBlock();
		return result;
	}

	std::string CodeGenerator::ParseVariableDeclerationStatement(const Variable& var)
	{
		std::string result;

		result += Indent() + ParseVariableDataTypeName(var.GetDataType()) + " " + var.GetName();

		switch (var.GetDataType().GetType())
		{
		case Type::INT:
		case Type::UINT:
			result += " = 0";
			break;
		case Type::DOUBLE:
			result += " = 0.0f";
			break;
		case Type::BOOL:
			result += " = false";
			break;
		default:
			break;
		}
		result += ";";
		result += NewLine();

		return result;
	}

	std::string CodeGenerator::ParseInputFuntionDefinition(const std::string& functionName, const std::vector<Variable>& params)
	{
		std::string result;
		result += "void " + functionName;
		result += ParseParameterList(params, true);
		result += NewLine();
		result += BeginBlock();

		for (Variable var : params)
		{
			result += GetInput(var);
		}

		result += CloseBlock();
		return result;
	}

	std::string CodeGenerator::ParseOutputFuntionDefinition(const std::string& functionName, const std::vector<Variable>& params)
	{
		std::string result;
		result += "void " + functionName;
		result += ParseParameterList(params);
		result += NewLine();
		result += BeginBlock();

		result += PrintOutput(params[0]);


		result += CloseBlock();
		return result;
	}

	std::string CodeGenerator::ParseMainFuntionDefinition()
	{
		std::string result;
		result += "int main()";
		result += NewLine();
		result += BeginBlock();
		result += ParseVariableDeclerationStatement(function.GetReturnVariable());

		for (Variable var : function.GetParameters())
		{
			result += ParseVariableDeclerationStatement(var);
		}

		result += ParseFunctionCallStatement("GetInput", function.GetParameters());


		result += Indent() + function.GetReturnVariable().GetName() + " = ";

		result += ParseFunctionCallStatement(function.GetName(), function.GetParameters(), 0);
		std::vector<Variable> args = { function.GetReturnVariable() };
		result += ParseFunctionCallStatement("PrintOutput", args);
		result += ParseReturnStatement(0);
		result += CloseBlock();
		return result;
	}

	std::string CodeGenerator::NewLine()
	{
		return "\n";
	}

	std::string CodeGenerator::BeginBlock()
	{
		return "{" + NewLine();
	}

	std::string CodeGenerator::CloseBlock()
	{
		return "}" + NewLine();
	}

	std::string CodeGenerator::Indent(size_t size)
	{
		std::string result;
		for (size_t index = 0; index < size; ++index)
			result += "\t";
		return result;
	}

	std::string CodeGenerator::GetInput(const Variable& var)
	{
		std::string result;
		result += Indent() + "std::cout << \"Nhap " + var.GetName() + ": \";";
		result += NewLine();
		result += Indent() + "std::cin >> " + var.GetName() + ";";
		result += NewLine();
		return result;
	}

	std::string CodeGenerator::PrintOutput(const Variable& var)
	{
		std::string result;
		result += Indent() + "std::cout << \"Ket qua la: \" << " + var.GetName() + " << std::endl;";
		result += NewLine();
		return result;
	}

	void CodeGenerator::AddHeaderInclude(const std::string& headerName)
	{
		headerIncludes.emplace_back(headerName);
	}

	std::string CodeGenerator::ParseStatement(const Statement& statement, size_t indent)
	{
		std::string result;

		if (statement.GetStatementType() == StatementType::EXPRESSION_ASSIGNMENT_CALL)
		{
			result += ParseAssignmentStatement(statement, indent);
		}
		else if (statement.GetStatementType() == StatementType::SELECTION_STATEMENT)
		{
			result += ParseIfStatement(statement, indent);
		}

		return result;
	}

	std::string CodeGenerator::ParseIfStatement(const Statement& statement, size_t indent)
	{
		std::string result;
		result += Indent(indent) + "if ";


		std::string expression = ParseOperatorCallStatement(statement.GetCompoundStatements()[0]);
		if (expression == "true" ||
			expression == "false")
		{
			result += "(" + expression + ")";
		}
		else
		{
			result += expression;
		}

		result += NewLine();
		result += Indent(indent) + BeginBlock();
		for (Statement statement : statement.GetCompoundStatements())
		{
			result += ParseStatement(statement, indent + 1);
		}
		result += Indent(indent) + CloseBlock();
		return result;
	}

	std::string CodeGenerator::ParseAssignmentStatement(const Statement& statement, size_t indent)
	{
		std::string result;
		result += Indent(indent);
		result += statement.GetCompoundStatements()[0].GetName() + " = ";

		result += ParseOperatorCallStatement(statement.GetCompoundStatements()[1]) + ";";
		result += NewLine();
		return result;
	}

	std::string CodeGenerator::ParseOperatorCallStatement(const Statement& statement)
	{
		std::string result;
		if (statement.GetCompoundStatements().size() != 2)
			return statement.GetName();

		std::string lhs;
		if (statement.GetCompoundStatements()[0].GetStatementType() == StatementType::EXPRESSION_OPERATOR_CALL)
			lhs = ParseOperatorCallStatement(statement.GetCompoundStatements()[0]);
		else
			lhs = statement.GetCompoundStatements()[0].GetName();

		std::string rhs;
		if (statement.GetCompoundStatements()[1].GetStatementType() == StatementType::EXPRESSION_OPERATOR_CALL)
			rhs = ParseOperatorCallStatement(statement.GetCompoundStatements()[1]);
		else
			rhs = statement.GetCompoundStatements()[1].GetName();

		if (statement.GetName().find(":") != -1)
		{
			std::string unaryOp = statement.GetName();
			unaryOp.pop_back();
			result += unaryOp + rhs;
		}
		else
		{
			result += "(" + lhs + " " + statement.GetName() + " " + rhs + ")";
		}

		return result;
	}

	std::string CodeGenerator::ParseReturnStatement(const Variable& var, size_t indent)
	{
		return Indent(indent) + "return " + var.GetName() + ";" + NewLine();
	}

	std::string CodeGenerator::ParseReturnStatement(const int code, size_t indent)
	{
		return Indent(indent) + "return " + std::to_string(code) + ";" + NewLine();
	}

	std::string CodeGenerator::ParseHeaderInclude()
	{
		std::string result;
		for (std::string header : headerIncludes)
		{
			result += "#include <" + header + ">" + NewLine();
		}
		return result;
	}

	std::string CodeGenerator::ParseFunctionCallStatement(const std::string& functionName, const std::vector<Variable>& params, size_t indent)
	{
		std::string result;
		result += Indent(indent);
		result += functionName + "(";

		for (size_t index = 0; index < params.size(); ++index)
		{
			result += params[index].GetName();

			if (index != params.size() - 1)
			{
				result += ", ";
			}
		}

		result += ");";
		result += NewLine();
		return result;
	}
}