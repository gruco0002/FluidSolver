#include "EngineException.hpp"

#include <string>

using namespace Engine;


const char* EngineException::what() const noexcept
{
    return ("Engine Exception: " + msg).c_str();
}

EngineException::EngineException(std::string msg) : msg(msg)
{
}
