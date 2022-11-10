#pragma once

#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "helpers/CompatibilityReport.hpp"
#include "helpers/DataChangeStruct.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"
#include "visualizer/Image.hpp"

#include <memory>

namespace LibFluid {

    class ISimulationVisualizer : public Initializable, public Reportable {
      public:
        struct Size {
            uint32_t width = 1920;
            uint32_t height = 1080;
        };


        struct SimulationData : public DataChangeStruct {
            std::shared_ptr<ParticleCollection> collection = nullptr;
            std::shared_ptr<NeighborhoodInterface> neighborhood_interface = nullptr;
            float rest_density = 0.0f;
            float particle_size = 0.0f;

        } simulation_data;

        struct VisualizerParameter : public DataChangeStruct {
            // this size specifies how large the render target should be in pixels
            Size render_target;
            bool enabled = false;

        } parameters;


        virtual void render() = 0;

        virtual Image get_image_data() = 0;
    };
} // namespace FluidSolver
