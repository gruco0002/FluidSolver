#include "SerializerBase.hpp"
namespace LibFluid::Serialization {

    SerializerBase::SerializerBase(SerializationContext& context) {
        this->context_ptr = &context;
    }

    SerializationContext& SerializerBase::context() {
        return *context_ptr;
    }
} // namespace LibFluid::Serialization