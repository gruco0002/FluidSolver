//
// Created by corbi on 16.04.2019.
//

#include <string>
#include "EngineException.hpp"

using namespace Engine;



const char *EngineException::what() const noexcept {
    return ("Engine Exception: " + msg).c_str();
}

EngineException::EngineException(std::string msg) : msg(msg) {}
