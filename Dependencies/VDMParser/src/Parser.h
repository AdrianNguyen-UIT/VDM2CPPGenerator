#pragma once
#include "OperatorPrecedence.h"
#include "CodeGenerator.h"
#include "Tokenizer.h"
#include "ErrorHandler.h"
namespace VDMParser
{
	class Parser
	{
	private:
		std::vector<Token> tokens;
		std::vector<Token>::iterator currentToken;
		std::vector<Token>::iterator endToken;
		std::vector<DataType> dataTypes;
		std::vector<OperatorPrecedence> opPrecedences;
		Function function;
		InputReader inputReader;

		bool inputIsEmpty;
		bool outputIsEmpty;
	public:
		Parser();

		void Read(const std::string& path);
		void SaveInput();
		std::optional<std::string> Parse();
		void ClearInput();
		void ClearOutput();

		const bool& InputIsEmpty() const;
		const bool& OutputIsEmpty() const;

		const std::string& GetSourceCode() const;
		std::string& GetSourceCode();
		const std::vector<Token>& GetTokens() const;
		const std::string& GetInputPath() const;

		void SetInput(const std::string& input);

		const std::string& GetFunctionName() const;

	private:
		void InitDataTypes();
		void InitOpPrecedence();
		bool ExpectFunctionDef();
		std::optional<Token> ExpectIdentifier(const std::string& name = std::string());
		std::optional<Token> ExpectOperator(const std::string& name = std::string());
		std::optional<DataType> ExpectDataType();
		std::optional<Variable> ExpectVarible();

		std::optional<std::vector<Statement>> ParseFunctionBody(const Function& func);
		std::optional<Statement> ExpectIfStatement(const Function& func);
		std::optional<Statement> ExpectFunctionCallOrVariableIdentifer(const Function& func);
		std::optional<Statement> ExpectExpression(const Function& func, bool virtualOpenBracket = false);
		std::optional<Statement> ExpectLiterals(const Function& func);

		bool IsArithmeticOperator(const std::string& op);
		bool IsRelationalOperator(const std::string& op);
		bool IsUnaryOperator(const std::string& op);
		bool IsAssigmentOperator(const std::string& op);
		bool IsKeyword(const std::string& name);
		
		std::optional<Token> ExpectTrueFalseIdentifer();

		Statement* FindRightMostStatement(Statement *lhs, size_t rhsPrecedence);
		size_t IsOpPrecedence(const std::string& opName);
	};
}