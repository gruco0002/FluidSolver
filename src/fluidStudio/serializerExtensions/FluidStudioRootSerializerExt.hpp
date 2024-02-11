#pragma once

#include "serialization/extensions/RootSerializerExtension.hpp"
#include "serializerExtensions/VseGlParticleRenderer.hpp"
#include "serializerExtensions/VseGlParticleRenderer3D.hpp"
#include "visualizer/GLRenderer.hpp"

#include <memory>

namespace FluidStudio
{
    class FluidStudioRootSerializerExt final : public LibFluid::Serialization::RootSerializerExtension
    {
      public:
        explicit FluidStudioRootSerializerExt(std::shared_ptr<LibFluid::GLRenderer> editor_visualizer);
        FluidStudioRootSerializerExt() = default;

        const char *get_node_name() const override;
        std::optional<nlohmann::json> serialize(LibFluid::Serialization::SerializationContext &context) override;
        void deserialize(std::optional<nlohmann::json> node,
                         LibFluid::Serialization::SerializationContext &context) override;

        std::shared_ptr<LibFluid::GLRenderer> get_editor_visualizer() const;

      private:
        VseGlParticleRenderer3D vse_gl_particle_renderer_3_d;
        VseGlParticleRenderer vse_gl_particle_renderer;

        std::shared_ptr<LibFluid::GLRenderer> editor_visualizer = nullptr;

        nlohmann::json serialize_editor_visualizer(LibFluid::Serialization::SerializationContext &context);
        void deserialize_editor_visualizer(const nlohmann::json &node,
                                           LibFluid::Serialization::SerializationContext &context);
    };
} // namespace FluidStudio
