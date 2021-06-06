#pragma once
#include <vector>
#include <stdexcept>
#include <optional>
#include "Token.h"
#include "InputReader.h"

namespace VDMParser
{
	class Tokenizer
	{
	public:
		std::optional<std::vector<Token>> Parse(InputReader &inputReader);
	private:
		void EndToken(Token &token, std::vector<Token> &tokens);
	};

}