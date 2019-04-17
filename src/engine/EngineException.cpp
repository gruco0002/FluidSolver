//
// Created by corbi on 16.04.2019.
//

#include <string>
#include "EngineException.hpp"

using namespace Engine;

EngineException::EngineException(char const *const _Message) : exception(_Message) {

}

const char *EngineException::what() const {
    return ("Engine Exception: " + std::string(exception::what())).c_str();
}
