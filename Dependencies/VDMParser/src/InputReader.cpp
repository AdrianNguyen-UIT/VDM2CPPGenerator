#include "InputReader.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace VDMParser
{
	InputReader::InputReader()
		: currentCharIndex(0)
	{
	}

	void InputReader::Read(const char* path)
	{
		std::ifstream sourceFile;

		sourceFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			sourceFile.open(path);

			std::stringstream stream;
			stream << sourceFile.rdbuf();
			sourceFile.close();

			sourceCode = stream.str();
			this->path = std::string(path);
		}
		catch (const std::ifstream::failure& e)
		{
			__debugbreak();
		}
	}

	void InputReader::Save()
	{
		std::ofstream out(path);

		try
		{
			if (out.is_open())
			{
				out << sourceCode;
				out.close();
			}
		}
		catch (const std::ofstream::failure& e)
		{
			__debugbreak();
		}
	}

	std::optional<char> InputReader::Consume()
	{
		if (currentCharIndex == sourceCode.length())
		{
			return {};
		}

		char result = sourceCode[currentCharIndex];

		++currentCharIndex;

		return result;
	}

	std::optional<char> InputReader::Peek()
	{
		if (currentCharIndex == sourceCode.length())
			return {};

		return sourceCode[currentCharIndex];
	}

	std::optional<char> InputReader::Peek(size_t k)
	{
		if (currentCharIndex == sourceCode.length())
			return {};

		if (currentCharIndex + k > sourceCode.length() - 1)
			return {};

		return sourceCode[currentCharIndex + k];
	}

	void InputReader::SetSourceCode(const std::string& input)
	{
		sourceCode = input;
	}

	const std::string& InputReader::GetSourceCode() const
	{
		return sourceCode;
	}

	std::string& InputReader::GetSourceCode()
	{
		return sourceCode;
	}

	const std::string& InputReader::GetPath() const
	{
		return path;
	}

	void InputReader::ResetCharIndex()
	{
		currentCharIndex = 0;
	}

	void InputReader::Clear()
	{
		currentCharIndex = 0;
		sourceCode.clear();
		path.clear();
	}
}