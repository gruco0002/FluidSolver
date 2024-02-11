#include "FluidStudioRootSerializerExt.hpp"

#include "LibFluidAssert.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

namespace FluidStudio
{

    const char *FluidStudioRootSerializerExt::get_node_name() const
    {
        return "fluid-studio";
    }

    std::optional<nlohmann::json> FluidStudioRootSerializerExt::serialize(
        LibFluid::Serialization::SerializationContext &context)
    {
        nlohmann::json node;

        // serialize editor visualizer
        context.begin_section("editor-visualizer");
        node["editor-visualizer"] = serialize_editor_visualizer(context);
        context.end_section();

        return node;
    }

    void FluidStudioRootSerializerExt::deserialize(std::optional<nlohmann::json> node,
                                                   LibFluid::Serialization::SerializationContext &context)
    {
        if (!node.has_value())
        {
            return;
        }
        auto &value = node.value();

        // deserialize editor visualizer
        if (value.contains("editor-visualizer"))
        {
            context.begin_section("editor-visualizer");
            deserialize_editor_visualizer(value["editor-visualizer"], context);
            context.end_section();
        }
    }

    FluidStudioRootSerializerExt::FluidStudioRootSerializerExt(std::shared_ptr<LibFluid::GLRenderer> editor_visualizer)
    {
        this->editor_visualizer = std::move(editor_visualizer);
    }

    std::shared_ptr<LibFluid::GLRenderer> FluidStudioRootSerializerExt::get_editor_visualizer() const
    {
        return editor_visualizer;
    }

    nlohmann::json FluidStudioRootSerializerExt::serialize_editor_visualizer(
        LibFluid::Serialization::SerializationContext &context)
    {
        auto gl_renderer_2d = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(editor_visualizer);
        auto gl_renderer_3d = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(editor_visualizer);
        FLUID_ASSERT(gl_renderer_2d != nullptr || gl_renderer_3d != nullptr);

        if (gl_renderer_2d)
        {
            return vse_gl_particle_renderer.serialize_visualizer(gl_renderer_2d, context);
        }
        else if (gl_renderer_3d)
        {
            return vse_gl_particle_renderer_3_d.serialize_visualizer(gl_renderer_3d, context);
        }

        return {};
    }

    void FluidStudioRootSerializerExt::deserialize_editor_visualizer(
        const nlohmann::json &node, LibFluid::Serialization::SerializationContext &context)
    {
        FLUID_ASSERT(node.contains("type"));

        auto type = node["type"].get<std::string>();
        if (type == "gl-particle-renderer")
        {
            editor_visualizer = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(
                vse_gl_particle_renderer.deserialize_visualizer(node, context));
        }
        else if (type == "gl-particle-renderer-3d")
        {
            editor_visualizer = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(
                vse_gl_particle_renderer_3_d.deserialize_visualizer(node, context));
        }
    }

} // namespace FluidStudio
