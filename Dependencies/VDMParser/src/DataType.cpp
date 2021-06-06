#include "DataType.h"

namespace VDMParser
{
	DataType::DataType(const std::string& pName, Type pType)
		: name(pName), type(pType)
	{
	}

	void DataType::Clear()
	{
		name.clear();
		type = Type::STRING;
	}

	void DataType::SetName(const std::string& pName)
	{
		name = pName;
	}

	void DataType::SetType(const Type& pType)
	{
		type = pType;
	}

	const std::string& DataType::GetName() const
	{
		return name;
	}

	const Type& DataType::GetType() const
	{
		return type;
	}
}