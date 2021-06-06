#pragma once
#include <string>
#include <vector>
namespace VDMParser
{
	class Error
	{
	private:
		std::string moduleName;
		std::string message;
		size_t lineNumber;

	public:
		Error(const std::string& pModuleName, const std::string& message, const size_t& pLineNumber);

		void SetModuleName(const std::string& pModuleName);
		void SetMessageLog(const std::string& pMessage);
		void SetLineNumber(const std::size_t& pLineNumber);

		const std::string& GetModuleName() const;
		const std::string& GetMessageLog() const;
		const size_t& GetLineNumber() const;
	};

	class ErrorHandler
	{
	private:
		static std::vector<Error> errors;

	public:
		ErrorHandler() = default;
		ErrorHandler(const ErrorHandler& p_Input) = delete;
		ErrorHandler& operator=(const ErrorHandler& p_Input) = delete;

		static void AddError(const std::string& pModuleName, const std::string& message, const size_t& pLineNumber);
		static void Clear();
		static const std::vector<Error>& GetErrors();
		static bool HasError();
	};
}