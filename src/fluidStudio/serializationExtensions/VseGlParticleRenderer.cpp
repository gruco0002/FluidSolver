#include "VseGlParticleRenderer.hpp"

#include "serialization/helpers/DynamicPointerIs.hpp"
#include "serialization/helpers/JsonHelpers.hpp"
#include "visualizer/GLParticleRenderer.hpp"

namespace FluidStudio {

    bool VseGlParticleRenderer::can_serialize(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer) const {
        return LibFluid::Serialization::dynamic_pointer_is<LibFluid::GLParticleRenderer>(visualizer);
    }
    nlohmann::json VseGlParticleRenderer::serialize_visualizer(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer, LibFluid::Serialization::SerializationContext& context) const {
        auto r = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(visualizer);
        FLUID_ASSERT(r != nullptr, "Called with invalid subtype of visualizer!");

        nlohmann::json node;

        node["type"] = "gl-particle-renderer";

        // default parameters
        node["viewport"]["left"] = r->settings.viewport.left;
        node["viewport"]["right"] = r->settings.viewport.right;
        node["viewport"]["top"] = r->settings.viewport.top;
        node["viewport"]["bottom"] = r->settings.viewport.bottom;
        node["render-target"]["width"] = r->parameters.render_target.width;
        node["render-target"]["height"] = r->parameters.render_target.height;

        // custom parameters for the particle renderer
        node["settings"]["top"]["value"] = r->settings.topValue;
        node["settings"]["top"]["color"] = r->settings.topColor;
        node["settings"]["bottom"]["value"] = r->settings.bottomValue;
        node["settings"]["bottom"]["color"] = r->settings.bottomColor;
        node["settings"]["value-selection"] = (int)r->settings.colorSelection;
        node["settings"]["colors"]["boundary"] = r->settings.boundaryParticleColor;
        node["settings"]["colors"]["background"] = r->settings.backgroundClearColor;
        node["settings"]["show-memory-location"] = r->settings.showMemoryLocation;

        return node;
    }
} // namespace FluidStudio