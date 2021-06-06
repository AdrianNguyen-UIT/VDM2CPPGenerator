#include "Token.h"

namespace VDMParser
{
	Token::Token()
		: type(TokenType::WHITESPACE), lineNumber(1)
	{
	}

	void Token::SetType(const TokenType& pType) 
	{ 
		type = pType; 
	}

	void Token::SetText(const std::string& pText)
	{ 
		text = pText; 
	}

	void Token::IncreLineNumber() 
	{ 
		++lineNumber; 
	}

	const TokenType& Token::GetType() const
	{ 
		return type; 
	}

	const std::string& Token::GetText() const
	{ 
		return text; 
	}

	const size_t& Token::GetLineNumber() const 
	{ 
		return lineNumber; 
	}

	void Token::AppendChar(const char& c) 
	{ 
		text.append(1, c); 
	}

	void Token::EraseText() 
	{
		text.erase(); 
	}
}