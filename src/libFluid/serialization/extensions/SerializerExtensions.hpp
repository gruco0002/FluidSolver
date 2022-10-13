#pragma once

#include <memory>
#include <vector>

namespace LibFluid::Serialization {

    // Forward declaration
    class VisualizerSerializerExtension;

    class SerializerExtensions {
      public:
        std::vector<std::shared_ptr<VisualizerSerializerExtension>> visualizer_serializer_extensions;
    };
} // namespace LibFluid::Serialization
