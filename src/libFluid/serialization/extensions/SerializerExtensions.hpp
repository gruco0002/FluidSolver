#pragma once

#include <memory>
#include <vector>

namespace LibFluid::Serialization {

    // Forward declaration
    class VisualizerSerializerExtension;
    class RootSerializerExtension;

    class SerializerExtensions {
      public:
        std::vector<std::shared_ptr<VisualizerSerializerExtension>> visualizer_serializer_extensions;
        std::vector<std::shared_ptr<RootSerializerExtension>> root_serializer_extensions;
    };
} // namespace LibFluid::Serialization
