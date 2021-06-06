#pragma once
#include <string>
namespace VDMParser
{
	class OperatorPrecedence
	{
	private:
		std::string name;
		size_t level;

	public:
		OperatorPrecedence(const std::string& pName = "", const size_t& pLevel = 1);

		void SetName(const std::string& pName);
		void SetLevel(const size_t& pLevel);

		const std::string& GetName() const;
		const size_t& GetLevel() const;
	};
}