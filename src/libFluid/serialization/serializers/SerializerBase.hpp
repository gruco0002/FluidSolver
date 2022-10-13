#pragma once

#include "serialization/extensions/SerializerExtensions.hpp"
#include "serialization/helpers/SerializationContext.hpp"

namespace LibFluid::Serialization {
    class SerializerBase {
      public:
        explicit SerializerBase(SerializationContext& context, SerializerExtensions& serializer_extensions);

        SerializationContext& context();

        SerializerExtensions& serializer_extensions();

      private:
        SerializationContext* context_ptr = nullptr;
        SerializerExtensions* serializer_extensions_ptr = nullptr;
    };
} // namespace LibFluid::Serialization
