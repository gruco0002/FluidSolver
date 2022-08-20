#include "ExtendedSimulationSerializer.hpp"

#include "Log.hpp"
#include "serialization/JsonHelpers.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

namespace FluidStudio {


    std::shared_ptr<LibFluid::ISimulationVisualizer> ExtendedSimulationSerializer::deserialize_unknown_visualizer(
            const nlohmann::json& node) {
        if (node["type"].get<std::string>() == "gl-particle-renderer") {
            if (!LibFluid::GLRenderer::is_opengl_available()) {
                LibFluid::Log::error("[LOADING] Visualizer is not supported in this context. OpenGL was not "
                                     "initialized but visualizer requires OpenGL!");
                return nullptr;
            }
            auto r = std::make_shared<LibFluid::GLParticleRenderer>();

            // default parameters
            r->settings.viewport.left = node["viewport"]["left"].get<float>();
            r->settings.viewport.right = node["viewport"]["right"].get<float>();
            r->settings.viewport.top = node["viewport"]["top"].get<float>();
            r->settings.viewport.bottom = node["viewport"]["bottom"].get<float>();
            r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
            r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();

            // custom paramters for the particle renderer
            r->settings.topValue = node["settings"]["top"]["value"].get<float>();
            r->settings.topColor = node["settings"]["top"]["color"].get<glm::vec4>();
            r->settings.bottomValue = node["settings"]["bottom"]["value"].get<float>();
            r->settings.bottomColor = node["settings"]["bottom"]["color"].get<glm::vec4>();
            r->settings.colorSelection =
                    (LibFluid::GLParticleRenderer::Settings::ColorSelection)node["settings"]["value-selection"]
                            .get<int>();
            r->settings.boundaryParticleColor = node["settings"]["colors"]["boundary"].get<glm::vec4>();
            r->settings.backgroundClearColor = node["settings"]["colors"]["background"].get<glm::vec4>();
            r->settings.showMemoryLocation = node["settings"]["show-memory-location"].get<bool>();

            return r;
        } else if (node["type"].get<std::string>() == "gl-particle-renderer-3d") {
            if (!LibFluid::GLRenderer::is_opengl_available()) {
                LibFluid::Log::error("[LOADING] Visualizer is not supported in this context. OpenGL was not "
                                     "initialized but visualizer requires OpenGL!");
                return nullptr;
            }
            auto r = std::make_shared<LibFluid::GLParticleRenderer3D>();

            // default parameters
            r->parameters.render_target.width = node["render-target"]["width"].get<size_t>();
            r->parameters.render_target.height = node["render-target"]["height"].get<size_t>();

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


        return nullptr;
    }

    std::optional<nlohmann::json> ExtendedSimulationSerializer::serialize_unknown_visualizer(
            const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer) {
        if (std::dynamic_pointer_cast<const LibFluid::GLParticleRenderer>(visualizer) != nullptr) {
            auto r = std::dynamic_pointer_cast<const LibFluid::GLParticleRenderer>(visualizer);
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
        } else if (std::dynamic_pointer_cast<const LibFluid::GLParticleRenderer3D>(visualizer) != nullptr) {
            auto r = std::dynamic_pointer_cast<const LibFluid::GLParticleRenderer3D>(visualizer);
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

        // return empty optional
        return {};
    }

} // namespace FluidStudio