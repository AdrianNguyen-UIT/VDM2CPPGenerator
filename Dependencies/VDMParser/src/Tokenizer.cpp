#include "Tokenizer.h"
#include "ErrorHandler.h"
namespace VDMParser
{
	std::optional<std::vector<Token>> VDMParser::Tokenizer::Parse(InputReader& reader)
	{
		Token currentToken;
		std::vector<Token> tokens;

		while (reader.Peek().has_value())
		{
			char currentChar = reader.Peek().value();

			if (currentToken.GetType() == TokenType::STRING_ESCAPE_SEQUENCE)
			{
				switch (currentChar)
				{
				case 'n':
					currentToken.AppendChar('\n');
					break;
				case 't':
					currentToken.AppendChar('\t');
					break;
				case 'r':
					currentToken.AppendChar('\t');
					break;
				case '\\':
					currentToken.AppendChar('\\');
					break;
				default:
					std::string message = std::string("Unknown escape sequence: \\") + std::string(1, currentChar);
					ErrorHandler::AddError("Tokenizer", message, currentToken.GetLineNumber());
					return {};
				}

				currentToken.SetType(TokenType::STRING_LITERAL);
				continue;
			}

			switch (currentChar)
			{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (currentToken.GetType() == TokenType::WHITESPACE)
				{
					currentToken.SetType(TokenType::INTEGER_LITERAL);
				}

				currentToken.AppendChar(currentChar);
				break;

			case '.':
				if (currentToken.GetType() != TokenType::STRING_LITERAL)
				{
					if (currentToken.GetType() == TokenType::INDENTIFIER)
					{
						EndToken(currentToken, tokens);
					}
					currentToken.SetType(TokenType::DOUBLE_LITERAL);
				}

				currentToken.AppendChar(currentChar);
				break;

			case '>':
			case '<':
			case '!':
			case '=':
				if (currentToken.GetType() != TokenType::STRING_LITERAL)
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::OPERATOR);
					currentToken.AppendChar(currentChar);

					if (reader.Peek(1).has_value())
					{
						char nextChar = reader.Peek(1).value();
						if (nextChar == '=')
						{
							currentToken.AppendChar('=');
							reader.Consume();
						}
					}
					EndToken(currentToken, tokens);
				}
				else
				{
					currentToken.AppendChar(currentChar);
				}
				break;

			case '&':
			case '|':
				if (currentToken.GetType() != TokenType::STRING_LITERAL)
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::OPERATOR);
					currentToken.AppendChar(currentChar);

					if (reader.Peek(1).has_value())
					{
						char nextChar = reader.Peek(1).value();
						if (nextChar == currentChar)
						{
							currentToken.AppendChar(currentChar);
							reader.Consume();
						}
					}
					EndToken(currentToken, tokens);
				}
				else
				{
					currentToken.AppendChar(currentChar);
				}
				break;

			case '(':
			case ')':
			case ',':
			case ':':
			case '+':
			case '-':
			case '/':
			case '%':
				if (currentToken.GetType() != TokenType::STRING_LITERAL)
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::OPERATOR);
					currentToken.AppendChar(currentChar);
					EndToken(currentToken, tokens);
				}
				else
				{
					currentToken.AppendChar(currentChar);
				}
				break;

			case '*':
				if (currentToken.GetType() != TokenType::STRING_LITERAL && currentToken.GetType() != TokenType::INDENTIFIER)
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::OPERATOR);
					currentToken.AppendChar(currentChar);
					EndToken(currentToken, tokens);
				}
				else 
				{
					currentToken.AppendChar(currentChar);
				}
				break;

			case ' ':
			case '\t':
				if (currentToken.GetType() == TokenType::STRING_LITERAL)
				{
					currentToken.AppendChar(currentChar);
				}
				else
				{
					EndToken(currentToken, tokens);
				}
				break;

			case '\n':
			case '\r':
				EndToken(currentToken, tokens);
				currentToken.IncreLineNumber();
				break;

			case '\\':
				if (currentToken.GetType() == TokenType::STRING_LITERAL)
				{
					currentToken.SetType(TokenType::STRING_ESCAPE_SEQUENCE);
				}
				else
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::OPERATOR);
					currentToken.AppendChar(currentChar);
					EndToken(currentToken, tokens);
				}
				break;

			case '"':
				if (currentToken.GetType() != TokenType::STRING_LITERAL)
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::STRING_LITERAL);
				}
				else if(currentToken.GetType() == TokenType::STRING_LITERAL)
				{
					EndToken(currentToken, tokens);
				}
				break;

			default:
				if (currentToken.GetType() == TokenType::WHITESPACE ||
					currentToken.GetType() == TokenType::INTEGER_LITERAL ||
					currentToken.GetType() == TokenType::DOUBLE_LITERAL)
				{
					EndToken(currentToken, tokens);
					currentToken.SetType(TokenType::INDENTIFIER);
					currentToken.AppendChar(currentChar);
				}
				else
				{
					currentToken.AppendChar(currentChar);
				}
				break;
			}

			reader.Consume();
		}

		EndToken(currentToken, tokens);
		reader.ResetCharIndex();
		return tokens;
	}

	void Tokenizer::EndToken(Token& token, std::vector<Token>& tokens)
	{
		if (token.GetType() != TokenType::WHITESPACE)
		{
			tokens.emplace_back(token);
		}
		token.SetType(TokenType::WHITESPACE);
		token.EraseText();
	}
}