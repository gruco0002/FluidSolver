//
// Created by corbi on 19.04.2019.
//

#include <engine/EngineException.hpp>
#include "Enums.hpp"

using namespace Engine;

uint8_t SizeOfComponentType(ComponentType type) {
    switch (type) {
        case ComponentTypeFloat:
            return 4;
        case ComponentTypeUInt:
            return 4;
        case ComponentTypeHalfFloat:
            return 2;
        case ComponentTypeByte:
            return 1;
        case ComponentTypeShort:
            return 2;
        case ComponentTypeUnsignedByte:
            return 1;
        default:
            throw EngineException("Type Error: Could not retrieve size of given type!");
    }

}