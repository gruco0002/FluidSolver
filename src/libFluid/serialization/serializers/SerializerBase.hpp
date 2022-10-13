#pragma once

#include "serialization/helpers/SerializationContext.hpp"

namespace LibFluid::Serialization {
    class SerializerBase {
      public:
        explicit SerializerBase(SerializationContext& context);

        SerializationContext& context();

      private:
        SerializationContext* context_ptr = nullptr;
    };
} // namespace LibFluid::Serialization
