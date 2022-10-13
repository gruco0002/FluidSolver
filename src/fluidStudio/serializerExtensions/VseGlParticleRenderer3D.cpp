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

} // namespace FluidStudio
