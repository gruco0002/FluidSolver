#include "SerializerBase.hpp"
namespace LibFluid::Serialization {


    SerializationContext& SerializerBase::context() {
        return *context_ptr;
    }

    SerializerBase::SerializerBase(SerializationContext& context, SerializerExtensions& serializer_extensions) {
        this->context_ptr = &context;
        this->serializer_extensions_ptr = &serializer_extensions;
    }

    SerializerExtensions& SerializerBase::serializer_extensions() {
        return *serializer_extensions_ptr;
    }
} // namespace LibFluid::Serialization