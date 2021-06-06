#include "Parser.h"
namespace VDMParser
{
	Parser::Parser()
		: inputIsEmpty(true), outputIsEmpty(true)
	{
		InitDataTypes();
		InitOpPrecedence();
	}

	void Parser::Read(const std::string& path)
	{
		inputReader.Read(path.c_str());
		inputIsEmpty = false;
	}

	void Parser::SaveInput()
	{
		inputReader.Save();
	}

	std::optional<std::string> Parser::Parse()
	{
		ClearOutput();

		Tokenizer tokenizer;
		std::optional<std::vector<Token>> possibleTokens = tokenizer.Parse(inputReader);

		if (!possibleTokens.has_value())
		{
			return {};
		}

		tokens = possibleTokens.value();
		currentToken = tokens.begin();
		endToken = tokens.end();

		CodeGenerator codeGenerator;
		if (currentToken != endToken)
		{
			if (ExpectFunctionDef())
			{
				outputIsEmpty = false;
				return codeGenerator.Generate(function);
			}
			return {};
		}
		outputIsEmpty = false;
		return codeGenerator.GenerateTemplate();
	}

	void Parser::ClearInput()
	{
		inputReader.Clear();
		inputIsEmpty = true;
	}

	void Parser::ClearOutput()
	{
		tokens.clear();
		function.Clear();
		outputIsEmpty = true;
	}

	const bool& Parser::InputIsEmpty() const
	{
		return inputIsEmpty;
	}

	const bool& Parser::OutputIsEmpty() const
	{
		return outputIsEmpty;
	}

	void Parser::InitDataTypes()
	{
		dataTypes.reserve(5);
		DataType type;
		type.SetName("Z");
		type.SetType(Type::INT);
		dataTypes.push_back(type);

		type.SetName("N");
		type.SetType(Type::UINT);
		dataTypes.push_back(type);

		type.SetName("R");
		type.SetType(Type::DOUBLE);
		dataTypes.push_back(type);

		type.SetName("B");
		type.SetType(Type::BOOL);
		dataTypes.push_back(type);

		type.SetName("char*");
		type.SetType(Type::STRING);
		dataTypes.push_back(type);
	}

	void Parser::InitOpPrecedence()
	{
		opPrecedences.reserve(17);

		OperatorPrecedence op;
		op.SetName("+:");
		op.SetLevel(1);
		opPrecedences.push_back(op);
		op.SetName("-:");
		op.SetLevel(1);
		opPrecedences.push_back(op);
		op.SetName("!:");
		op.SetLevel(1);
		opPrecedences.push_back(op);

		op.SetName("*");
		op.SetLevel(2);
		opPrecedences.push_back(op);
		op.SetName("/");
		op.SetLevel(2);
		opPrecedences.push_back(op);
		op.SetName("%");
		op.SetLevel(2);
		opPrecedences.push_back(op);

		op.SetName("+");
		op.SetLevel(3);
		opPrecedences.push_back(op);
		op.SetName("-");
		op.SetLevel(3);
		opPrecedences.push_back(op);

		op.SetName("<");
		op.SetLevel(4);
		opPrecedences.push_back(op);
		op.SetName(">");
		op.SetLevel(4);
		opPrecedences.push_back(op);
		op.SetName("<=");
		op.SetLevel(4);
		opPrecedences.push_back(op);
		op.SetName(">=");
		op.SetLevel(4);
		opPrecedences.push_back(op);
		op.SetName("==");
		op.SetLevel(4);
		opPrecedences.push_back(op);
		op.SetName("!=");
		op.SetLevel(4);
		opPrecedences.push_back(op);

		op.SetName("&&");
		op.SetLevel(5);
		opPrecedences.push_back(op);
		op.SetName("||");
		op.SetLevel(5);
		opPrecedences.push_back(op);

		op.SetName("=");
		op.SetLevel(6);
		opPrecedences.push_back(op);
	}

	const std::string& Parser::GetSourceCode() const
	{
		return inputReader.GetSourceCode();
	}

	std::string& Parser::GetSourceCode()
	{
		return inputReader.GetSourceCode();
	}

	const std::vector<Token>& Parser::GetTokens() const
	{
		return tokens;
	}

	const std::string& Parser::GetInputPath() const
	{
		return inputReader.GetPath();
	}

	void Parser::SetInput(const std::string& input)
	{
		inputReader.SetSourceCode(input);
	}

	const std::string& Parser::GetFunctionName() const
	{
		return function.GetName();
	}

	std::optional<Token> Parser::ExpectIdentifier(const std::string& name)
	{
		if (currentToken == endToken)
		{
			return {};
		}

		if (currentToken->GetType() != TokenType::INDENTIFIER)
		{
			return{};
		}

		if (!name.empty() && currentToken->GetText() != name)
		{
			return{};
		}

		Token returnToken = *currentToken;
		++currentToken;

		return returnToken;
	}

	std::optional<Token> Parser::ExpectOperator(const std::string& name)
	{
		if (currentToken == endToken)
		{
			return {};
		}

		if (currentToken->GetType() != TokenType::OPERATOR)
		{
			return{};
		}

		if (!name.empty() && currentToken->GetText() != name)
		{
			return{};
		}

		Token returnToken = *currentToken;
		++currentToken;

		return returnToken;
	}

	std::optional<DataType> Parser::ExpectDataType()
	{
		std::optional<Token> possibleIndentifier = ExpectIdentifier();

		if (!possibleIndentifier.has_value())
		{
			return {};
		}

		for (DataType dataType : dataTypes)
		{
			if (dataType.GetName() == possibleIndentifier->GetText())
			{
				return dataType;
			}
		}

		--currentToken;
		std::string message = std::string("Unknown data type: ") + possibleIndentifier->GetText();
		ErrorHandler::AddError("Parser", message, currentToken->GetLineNumber());
		return {};

	}

	bool Parser::ExpectFunctionDef()
	{
		std::vector<Token>::iterator parseStart = currentToken;

		std::optional<Token> possibleName = ExpectIdentifier();

		if (possibleName.has_value())
		{
			std::optional<Token> possibleSeperator = ExpectOperator("(");

			if (possibleSeperator.has_value())
			{
				function.SetName(possibleName->GetText());

				while (!ExpectOperator(")").has_value()) 
				{
					std::optional<Variable> possibleVar = ExpectVarible();
					if (ErrorHandler::HasError())
					{
						return false;
					}

					if (!possibleVar.has_value())
					{
						ErrorHandler::AddError("Parser", "Expect parameter.", currentToken->GetLineNumber());
						return false;
					}

					function.AddParameter(possibleVar.value());

					if (ExpectOperator(")").has_value()) 
					{
						break;
					}

					if (!ExpectOperator(",").has_value()) 
					{
						ErrorHandler::AddError("Parser", "Expect ',' to separate parameters or ')' to indicate end of argument list.", currentToken->GetLineNumber());
						return false;
					}
				}

				std::optional<Variable> possibleReturnVar = ExpectVarible();

				if (ErrorHandler::HasError())
				{
					return false;
				}

				if (!possibleReturnVar.has_value())
				{
					ErrorHandler::AddError("Parser", "Expect return variable.", currentToken->GetLineNumber());
					return false;
				}
					
				function.SetReturnVarible(possibleReturnVar.value());

				std::optional<std::vector<Statement>> statements = ParseFunctionBody(function);

				if (ErrorHandler::HasError())
				{
					return false;
				}

				if (statements.has_value()) 
				{
					function.GetStatements().insert(function.GetStatements().begin(), statements->begin(), statements->end());
				}
				return true;
			}
			else
			{
				currentToken = parseStart;
				ErrorHandler::AddError("Parser", "Expect '(' for parameter(s) decleration.", currentToken->GetLineNumber());
			}
		}
		else
		{
			ErrorHandler::AddError("Parser", "Expect function name.", currentToken->GetLineNumber());
			currentToken = parseStart;
		}

		return false;
	}

	std::optional<Variable> Parser::ExpectVarible()
	{
		std::optional<Token> possibleVariableName = ExpectIdentifier();
		if (!possibleVariableName.has_value())
		{
			return {};
		}

		if (!ExpectOperator(":").has_value())
		{
			ErrorHandler::AddError("Parser", "Expect ':' in variable decleration.", currentToken->GetLineNumber());
			return {};
		}

		std::optional<DataType> possibleVariableDataType = ExpectDataType();
		if (!possibleVariableDataType.has_value()) 
		{
			return {};
		}

		Variable var;
		var.SetName(possibleVariableName->GetText());
		var.SetDataType(possibleVariableDataType.value());

		return var;
	}

	std::optional<std::vector<Statement>> Parser::ParseFunctionBody(const Function& func)
	{
		std::vector<Statement> statements;
		if (ExpectIdentifier("pre").has_value())
		{
			if (!ExpectIdentifier("post").has_value())
			{
				std::optional<Statement> conditionExpression = ExpectExpression(func);
				if (ErrorHandler::HasError())
				{
					return {};
				}

				if (conditionExpression.has_value())
				{
					Statement preIfStatement;
					preIfStatement.SetStatementType(StatementType::SELECTION_STATEMENT);
					preIfStatement.AddInnerStatement(conditionExpression.value());

					if (ExpectIdentifier("post").has_value())
					{
						if (currentToken == endToken)
						{
							statements.emplace_back(preIfStatement);
							return statements;
						}

						do
						{
							std::optional<Statement> possibleIfStatement = ExpectIfStatement(func);
							if (ErrorHandler::HasError())
							{
								return {};
							}

							if (possibleIfStatement.has_value())
							{
								preIfStatement.AddInnerStatement(possibleIfStatement.value());
							}
							else
							{
								break;
							}
						}
						while (ExpectOperator("||").has_value());
					}
					else
					{
						ErrorHandler::AddError("Parser", "Missing 'post' identifier.", currentToken->GetLineNumber());
					}
					statements.emplace_back(preIfStatement);
				}
			}
			else
			{
				do
				{
					if (currentToken == endToken)
					{
						return statements;
					}

					std::optional<Statement> possibleIfStatement = ExpectIfStatement(func);
					if (ErrorHandler::HasError())
					{
						return {};
					}

					if (possibleIfStatement.has_value())
					{
						statements.emplace_back(possibleIfStatement.value());
					}
					else
					{
						break;
					}
				}
				while (ExpectOperator("||").has_value());
			}
		}
		else
		{
			ErrorHandler::AddError("Parser", "Missing 'pre' identifier.", currentToken->GetLineNumber());
		}
		return statements;
	}

	std::optional<Statement> Parser::ExpectIfStatement(const Function& func)
	{
		Statement result;
		//Expect Then Statements
		std::optional<Statement> assignmentStatement = ExpectExpression(func, false);
		if (ErrorHandler::HasError())
		{
			return {};
		}
		//Expect Condition
		std::optional<Statement> condition;
		if (ExpectOperator("&&").has_value())
		{
			condition = ExpectExpression(func, true);
			if (ErrorHandler::HasError())
			{
				return {};
			}
		}

		if (!condition.has_value()) 
		{
			if (assignmentStatement.has_value())
			{
				return assignmentStatement.value();
			}
			return {};
		}

		result.SetStatementType(StatementType::SELECTION_STATEMENT);
		result.AddInnerStatement(condition.value());

		if (assignmentStatement.has_value())
		{
			result.AddInnerStatement(assignmentStatement.value());
		}

		return result;
	}

	std::optional<Statement> Parser::ExpectFunctionCallOrVariableIdentifer(const Function& func)
	{
		std::vector<Token>::iterator startToken = currentToken;

		std::optional<Token> possibleFunctionNameOrVarIdentifer = ExpectIdentifier();
		if (!possibleFunctionNameOrVarIdentifer.has_value()) 
		{
			currentToken = startToken;
			return {};
		}

		std::optional<Token> possibleOperator = ExpectOperator();
		if (possibleOperator.has_value())
		{
			if (possibleOperator->GetText() == "(")
			{
				Statement functionCall;
				functionCall.SetStatementType(StatementType::EXPRESSION_FUNCTION_CALL);
				functionCall.SetName(possibleFunctionNameOrVarIdentifer->GetText());

				while (!ExpectOperator(")").has_value()) 
				{
					std::optional<Statement> arg = ExpectExpression(func);
					if (!arg.has_value()) 
					{
						ErrorHandler::AddError("Parser", "Expect expression as parameter.", currentToken->GetLineNumber());
						return {};
					}
					functionCall.AddInnerStatement(arg.value());

					if (ExpectOperator(")").has_value()) 
					{
						break;
					}
					if (!ExpectOperator(",").has_value()) 
					{
						std::string message = std::string("Expect ',' to separate parameters, found '") + currentToken->GetText() + "'.";
						ErrorHandler::AddError("Parser", message, currentToken->GetLineNumber());
						return {};
					}
				}
				return functionCall;
			}
			currentToken = startToken;
		}

		//var identifer
		if (func.GetReturnVariable().GetName() == possibleFunctionNameOrVarIdentifer->GetText())
		{
			Statement varIdentiferStatement;
			varIdentiferStatement.SetStatementType(StatementType::EXPRESSION_VARIABLE_IDENTIFER);
			varIdentiferStatement.SetName(possibleFunctionNameOrVarIdentifer->GetText());
			varIdentiferStatement.SetDataType(func.GetReturnVariable().GetDataType());
			++currentToken;
			return varIdentiferStatement;
		}

		for (Variable var : func.GetParameters())
		{
			if (var.GetName() == possibleFunctionNameOrVarIdentifer->GetText())
			{
				Statement varIdentiferStatement;
				varIdentiferStatement.SetStatementType(StatementType::EXPRESSION_VARIABLE_IDENTIFER);
				varIdentiferStatement.SetName(var.GetName());
				varIdentiferStatement.SetDataType(var.GetDataType());
				++currentToken;
				return varIdentiferStatement;
			}
		}

		std::string message = std::string("Unknown variable: ") + currentToken->GetText() + ".";
		ErrorHandler::AddError("Parser", message, currentToken->GetLineNumber());
		return {};
	}

	std::optional<Statement> Parser::ExpectExpression(const Function& func, bool virtualOpenBracket)
	{
		bool hasOpenBracket = false;
		if (!virtualOpenBracket)
		{
			if (ExpectOperator("(").has_value())
				hasOpenBracket = true;
		}
		else
		{
			hasOpenBracket = true;
		}
		std::optional<Statement> lhs;

		std::optional<Token> unaryOp = ExpectOperator();

		if (unaryOp.has_value())
		{
			if (IsUnaryOperator(unaryOp->GetText()))
			{
				std::optional<Statement> operand = ExpectLiterals(func);
				if (ErrorHandler::HasError())
				{
					return {};
				}

				if (!operand.has_value())
				{
					ErrorHandler::AddError("Parser", "Expect expression as operand of unary operator.", currentToken->GetLineNumber());
					return {};
				}

				Statement operatorCall;
				operatorCall.SetStatementType(StatementType::EXPRESSION_OPERATOR_CALL);
				std::string modifiedUnaryOpText = unaryOp->GetText();
				modifiedUnaryOpText.append(1, ':');
				operatorCall.SetName(modifiedUnaryOpText);
				operatorCall.AddInnerStatement(Statement());
				operatorCall.AddInnerStatement(operand.value());
				lhs = operatorCall;
			}
			else if (IsAssigmentOperator(unaryOp->GetText()) || 
				IsRelationalOperator(unaryOp->GetText()) || 
				IsArithmeticOperator(unaryOp->GetText()) ||
				unaryOp->GetText() == ")" ||
				unaryOp->GetText() == ":" || 
				unaryOp->GetText() == "," )
			{
				ErrorHandler::AddError("Parser", "Expect a literal or an expression.", currentToken->GetLineNumber());
				return {};
			}
			else
			{
				--currentToken;
			}
		}

		if (!lhs.has_value())
		{
			lhs = ExpectLiterals(func);

			if (ErrorHandler::HasError())
			{
				return {};
			}
		}

		if (!lhs.has_value())
		{
			return {};
		}

		if (lhs->GetStatementType() == StatementType::EXPRESSION_ASSIGNMENT_CALL)
		{
			return lhs;
		}

		while (true)
		{
			std::optional<Token> op = ExpectOperator();
			if (!op.has_value()) 
			{ 
				break; 
			}
			size_t rhsPrecedence = IsOpPrecedence(op->GetText());
			if (rhsPrecedence == 0)
			{
				if (!hasOpenBracket)
					--currentToken;
				return lhs;
			}
			std::optional<Statement> rhs;
			if (IsAssigmentOperator(op->GetText()))
			{
				rhs = ExpectExpression(func);
			}
			else
			{
				rhs = ExpectLiterals(func);
			}

			if (ErrorHandler::HasError())
			{
				return {};
			}

			if (!rhs.has_value())
			{
				--currentToken;
				return lhs;
			}

			Statement operatorCall;
			if (IsAssigmentOperator(op->GetText()))
			{
				operatorCall.SetStatementType(StatementType::EXPRESSION_ASSIGNMENT_CALL);
			}
			else
			{
				operatorCall.SetStatementType(StatementType::EXPRESSION_OPERATOR_CALL);
			}
			operatorCall.SetName(op->GetText());

			Statement* rightmostStatement = FindRightMostStatement(&lhs.value(), rhsPrecedence);
			if (rightmostStatement)
			{
				operatorCall.AddInnerStatement(rightmostStatement->GetCompoundStatements().at(1));
				operatorCall.AddInnerStatement(rhs.value());
				rightmostStatement->GetCompoundStatements()[1] = operatorCall;
			}
			else
			{
				operatorCall.AddInnerStatement(lhs.value());
				operatorCall.AddInnerStatement(rhs.value());
				lhs = operatorCall;
			}
		}

		if (hasOpenBracket)
		{
			if (!ExpectOperator(")").has_value())
			{
				ErrorHandler::AddError("Parser", "Unbalanced '(' in parenthesized expression.", currentToken->GetLineNumber());
				return {};
			}
		}
		return lhs;
	}

	std::optional<Statement> Parser::ExpectLiterals(const Function& func)
	{
		std::optional<Statement> result;
		if (currentToken != endToken && currentToken->GetType() == TokenType::DOUBLE_LITERAL) 
		{
			Statement doubleLiteralStatement;
			doubleLiteralStatement.SetStatementType(StatementType::EXPRESSION_LITERAL);
			doubleLiteralStatement.SetName(currentToken->GetText());
			doubleLiteralStatement.SetDataType(DataType("R", Type::DOUBLE));
			result = doubleLiteralStatement;
			++currentToken;
		}
		else if (currentToken != endToken && currentToken->GetType() == TokenType::INTEGER_LITERAL) 
		{
			Statement integerLiteralStatement;
			integerLiteralStatement.SetStatementType(StatementType::EXPRESSION_LITERAL);
			integerLiteralStatement.SetName(currentToken->GetText());
			integerLiteralStatement.SetName(currentToken->GetText());
			integerLiteralStatement.SetDataType(DataType("Z", Type::INT));
			result = integerLiteralStatement;
			++currentToken;
		}
		else if (currentToken != endToken && currentToken->GetType() == TokenType::STRING_LITERAL) 
		{
			Statement stringLiteralStatement;
			stringLiteralStatement.SetStatementType(StatementType::EXPRESSION_LITERAL);
			std::string name;
			name += "\"" + currentToken->GetText() + "\"";
			stringLiteralStatement.SetName(name);
			stringLiteralStatement.SetDataType(DataType("char*", Type::STRING));
			result = stringLiteralStatement;
			++currentToken;
		}
		else
		{
			result = ExpectFunctionCallOrVariableIdentifer(func);
		}

		if (ErrorHandler::HasError())
		{
			return {};
		}

		if (!result.has_value()) 
		{
			result = ExpectExpression(func);

			if (ErrorHandler::HasError())
			{
				return {};
			}
		}
		return result;
	}

	bool Parser::IsArithmeticOperator(const std::string& op)
	{
		if (op == "+" ||
			op == "-" ||
			op == "*" ||
			op == "/" ||
			op == "%")
			return true;
		return false;
	}

	bool Parser::IsRelationalOperator(const std::string& op)
	{
		if (op == "!=" ||
			op == "==" ||
			op == ">" ||
			op == "<" ||
			op == ">="||
			op == "<=")
			return true;
		return false;
	}

	bool Parser::IsUnaryOperator(const std::string& op)
	{
		if (op == "+" ||
			op == "-" ||
			op == "!")
			return true;
		return false;
	}

	bool Parser::IsAssigmentOperator(const std::string& op)
	{
		if (op == "=")
			return true;
		return false;
	}

	bool Parser::IsKeyword(const std::string& name)
	{
		if (name == "pre" ||
			name == "post")
			return true;
		return false;
	}

	Statement* Parser::FindRightMostStatement(Statement* lhs, size_t rhsPrecedence)
	{
		if (lhs->GetStatementType() != StatementType::EXPRESSION_OPERATOR_CALL) { return nullptr; }
		if (IsOpPrecedence(lhs->GetName()) <= rhsPrecedence) { return nullptr; }

		Statement* rhs = &lhs->GetCompoundStatements().at(1);
		rhs = FindRightMostStatement(rhs, rhsPrecedence);
		if (rhs == nullptr) { return lhs; }
		return rhs;
	}

	size_t Parser::IsOpPrecedence(const std::string& opName)
	{
		for (OperatorPrecedence opPrecedence : opPrecedences)
		{
			if (opName == opPrecedence.GetName())
			{
				return opPrecedence.GetLevel();
			}
		}

		return 0;
	}
}