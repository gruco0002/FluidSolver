#include "FluidStudioRootSerializerExt.hpp"

namespace FluidStudio {


    const char* FluidStudioRootSerializerExt::get_node_name() const {
        return "fluid-studio";
    }

    std::optional<nlohmann::json> FluidStudioRootSerializerExt::serialize(LibFluid::Serialization::SerializationContext& context) const {
        nlohmann::json node;

        // TODO: implement
        // serialize editor visualizer

        return node;
    }

    void FluidStudioRootSerializerExt::deserialize(std::optional<nlohmann::json> node, LibFluid::Serialization::SerializationContext& context) const {
        if (!node.has_value()) {
            return;
        }
        auto& value = node.value();

        // TODO: implement
        // deserialize editor visualizer
    }

} // namespace FluidStudio