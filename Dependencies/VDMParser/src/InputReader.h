#pragma once
#include <string>
#include <optional>

namespace VDMParser 
{
	class InputReader
	{
	private:
		std::string sourceCode;
		size_t currentCharIndex;
		std::string path;
	public:
		InputReader();
		void Read(const char* path);
		void Save();
		std::optional<char> Consume();
		std::optional<char> Peek();
		std::optional<char> Peek(size_t token);

		void SetSourceCode(const std::string& input);

		const std::string& GetSourceCode() const;
		std::string& GetSourceCode();
		const std::string& GetPath() const;

		void ResetCharIndex();

		void Clear();
	};
}