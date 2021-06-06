#include "Variable.h"

namespace VDMParser
{
    Variable::Variable(const std::string& pName)
        : name(pName)
    {
    }

    void Variable::Clear()
    {
        name.clear();
        dataType.Clear();
    }

    void Variable::SetName(const std::string& pName)
    { 
        name = pName; 
    }

    void Variable::SetDataType(const DataType& pType)
    { 
        dataType = pType; 
    }

    const std::string& Variable::GetName() const
    { 
        return name; 
    }

    const DataType& Variable::GetDataType() const
    {
        return dataType;
    }
}