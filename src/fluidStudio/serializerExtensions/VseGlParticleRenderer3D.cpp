#include "VseGlParticleRenderer3D.hpp"

#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

namespace FluidStudio {

    bool VseGlParticleRenderer3D::can_serialize(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer) const {
        return LibFluid::Serialization::dynamic_pointer_is<LibFluid::GLParticleRenderer3D>(visualizer);
    }

    nlohmann::json VseGlParticleRenderer3D::serialize_visualizer(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer, LibFluid::Serialization::SerializationContext& context) const {
        auto r = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(visualizer);
        FLUID_ASSERT(r != nullptr, "Called with invalid subtype of visualizer!");

        nlohmann::json node;

        node["type"] = "gl-particle-renderer-3d";

        // default parameters
        node["render-target"]["width"] = r->parameters.render_target.width;
        node["render-target"]["height"] = r->parameters.render_target.height;
        node["enabled"] = r->parameters.enabled;

        // custom parameters for the particle renderer
        node["settings"]["background-color"] = r->settings.background_color;
        node["settings"]["boundary-color"] = r->settings.boundary_particle_color;
        node["settings"]["fluid-color"] = r->settings.fluid_particle_color;
        node["settings"]["light-direction"] = r->settings.light_direction;
        node["settings"]["camera"]["location"] = r->settings.camera.location;
        node["settings"]["camera"]["up"] = r->settings.camera.up;
        node["settings"]["camera"]["looking-at"] = r->settings.camera.looking_at;

        return node;
    }
    bool VseGlParticleRenderer3D::can_deserialize(const nlohmann::json& node) const {
        auto type = node["type"].get<std::string>();
        return type == "gl-particle-renderer-3d";
    }
    std::shared_ptr<LibFluid::ISimulationVisualizer> VseGlParticleRenderer3D::deserialize_visualizer(const nlohmann::json& node, LibFluid::Serialization::SerializationContext& context) const {
        if (!LibFluid::GLRenderer::is_opengl_available()) {
            context.add_issue("Visualizer is not supported in this context. OpenGL was not initialized but visualizer requires OpenGL!");
            return nullptr;
        }

        auto r = std::make_shared<LibFluid::GLParticleRenderer3D>();

        // default parameters
        r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
        r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();
        if (node.contains("enabled")) {
            r->parameters.enabled = node["enabled"].get<bool>();
        } else {
            r->parameters.enabled = true;
        }

        // custom paramters for the particle renderer
        r->settings.background_color = node["settings"]["background-color"].get<glm::vec4>();
        r->settings.boundary_particle_color = node["settings"]["boundary-color"].get<glm::vec4>();
        r->settings.fluid_particle_color = node["settings"]["fluid-color"].get<glm::vec4>();
        r->settings.light_direction = node["settings"]["light-direction"].get<glm::vec3>();
        r->settings.camera.location = node["settings"]["camera"]["location"].get<glm::vec3>();
        r->settings.camera.up = node["settings"]["camera"]["up"].get<glm::vec3>();
        r->settings.camera.looking_at = node["settings"]["camera"]["looking-at"].get<glm::vec3>();

        return r;
    }

} // namespace FluidStudio
