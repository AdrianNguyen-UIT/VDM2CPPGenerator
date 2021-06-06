#pragma once
#include "DataType.h"
#include <string>
namespace VDMParser
{
	class Variable
	{
	private:
		std::string name;
		DataType dataType;

	public:
		Variable(const std::string &name = "");

		void Clear();

		void SetName(const std::string& pName);
		void SetDataType(const DataType& pType);

		const std::string& GetName() const;
		const DataType& GetDataType() const;
	};
}