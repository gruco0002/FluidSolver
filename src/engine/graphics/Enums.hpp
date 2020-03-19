#ifndef CPPGUI_DEMO_ENUMS_HPP
#define CPPGUI_DEMO_ENUMS_HPP

#include <glad/glad.h>
#include <cstdint>

namespace Engine {


    enum ComponentType {
        ComponentTypeFloat = GL_FLOAT,
        ComponentTypeUInt = GL_UNSIGNED_INT,
        ComponentTypeHalfFloat = GL_HALF_FLOAT,
        ComponentTypeByte = GL_BYTE,
        ComponentTypeShort = GL_SHORT,
        ComponentTypeUnsignedByte = GL_UNSIGNED_BYTE,
    };

    uint8_t SizeOfComponentType(ComponentType type);

}
#endif //CPPGUI_DEMO_ENUMS_HPP
