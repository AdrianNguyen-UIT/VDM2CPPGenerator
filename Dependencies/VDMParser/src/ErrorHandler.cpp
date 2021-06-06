#include "ErrorHandler.h"
namespace VDMParser
{
	std::vector<Error> ErrorHandler::errors;

	void ErrorHandler::AddError(const std::string& pModuleName, const std::string& pMessage, const size_t& pLineNumber)
	{
		Error error(pModuleName, pMessage, pLineNumber);
		errors.push_back(error);
	}

	void ErrorHandler::Clear()
	{
		errors.clear();
	}

	const std::vector<Error>& ErrorHandler::GetErrors()
	{
		return errors;
	}

	bool ErrorHandler::HasError()
	{
		if (errors.size() != 0)
			return true;
		return false;
	}

	void Error::SetModuleName(const std::string& pModuleName)
	{
		moduleName = pModuleName;
	}

	void Error::SetMessageLog(const std::string& pMessage)
	{
		message = pMessage;
	}

	void Error::SetLineNumber(const std::size_t& pLineNumber)
	{
		lineNumber = pLineNumber;
	}

	const std::string& Error::GetModuleName() const
	{
		return moduleName;
	}

	const std::string& Error::GetMessageLog() const
	{
		return message;
	}

	const size_t& Error::GetLineNumber() const
	{
		return lineNumber;
	}

	Error::Error(const std::string& pModuleName, const std::string& pMessage, const size_t& pLineNumber)
		: moduleName(pModuleName), message(pMessage), lineNumber(pLineNumber)
	{
	}
}