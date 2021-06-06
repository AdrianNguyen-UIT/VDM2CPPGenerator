#pragma once
#include <string>
namespace VDMParser
{
	enum class TokenType 
	{
		WHITESPACE,
		INDENTIFIER,
		INTEGER_LITERAL,
		DOUBLE_LITERAL,
		STRING_LITERAL,
		OPERATOR,
		STRING_ESCAPE_SEQUENCE

	};


	class Token
	{
	private:
		TokenType type;
		std::string text;
		size_t lineNumber;

	public:
		Token();

		void SetType(const TokenType& pType);
		void SetText(const std::string& pText);
		void IncreLineNumber();

		const TokenType& GetType() const;
		const std::string& GetText() const;
		const size_t& GetLineNumber() const;

		void AppendChar(const char& c);
		void EraseText();
	};
}