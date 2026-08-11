#ifndef __UT_ROBOT_SDK_SERIALIZE_HPP__
#define __UT_ROBOT_SDK_SERIALIZE_HPP__

#include <unitree/common/json/jsonize.hpp>

namespace unitree
{
namespace robot
{
template<typename T>
inline bool Serialize(const T& instance, std::string& serialziedData, std::string* errorMessage = NULL)
{
    try
    {
        serialziedData = common::ToJsonString(instance);
    }
    catch(const common::Exception& e)
    {
        if (errorMessage != NULL)
        {
            *errorMessage = e.ToString();
        }

        return false;
    }

    return true;
}

template<typename T>
inline bool Deserialize(const std::string& serialziedData, T& instance, std::string* errorMessage = NULL)
{
    try
    {
        instance = common::FromJsonString<T>(serialziedData);
    }
    catch(const common::Exception& e)
    {
        if (errorMessage != NULL)
        {
            *errorMessage = e.ToString();
        }

        return false;
    }

    return true;
}

}
}

#endif//__UT_ROBOT_SDK_SERIALIZE_HPP__
