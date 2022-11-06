#pragma once

#include "serialization/extensions/RootSerializerExtension.hpp"
#include "serializerExtensions/VseGlParticleRenderer.hpp"
#include "serializerExtensions/VseGlParticleRenderer3D.hpp"

namespace FluidStudio {
    class FluidStudioRootSerializerExt final : public LibFluid::Serialization::RootSerializerExtension {
      public:
        const char* get_node_name() const override;
        std::optional<nlohmann::json> serialize(LibFluid::Serialization::SerializationContext& context) const override;
        void deserialize(std::optional<nlohmann::json> node, LibFluid::Serialization::SerializationContext& context) const override;

      private:
        VseGlParticleRenderer3D vse_gl_particle_renderer_3_d;
        VseGlParticleRenderer vse_gl_particle_renderer;
    };
} // namespace FluidStudio
