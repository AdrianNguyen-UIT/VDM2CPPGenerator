#pragma once
#include <string>
namespace VDMParser
{
	enum class Type
	{
		INT,
		DOUBLE,
		UINT,
		BOOL,
		STRING,
		VOID
	};

	class DataType
	{
	private:
		std::string name;
		Type type;

	public:
		DataType(const std::string &pName = "", Type pType = Type::STRING);

		void Clear();

		void SetName(const std::string& pName);
		void SetType(const Type& pType);

		const std::string& GetName() const;
		const Type& GetType() const;
	};

}