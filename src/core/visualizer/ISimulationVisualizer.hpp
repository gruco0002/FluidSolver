#ifndef FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
#define FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP

#include "core/fluidSolver/ParticleCollection.hpp"
#include "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "core/visualizer/Image.hpp"

#include <memory>

namespace FluidSolver
{

    class ISimulationVisualizer {
      public:
        struct Size
        {
            uint32_t width = 1920;
            uint32_t height = 1080;
        };


        struct VisualizerParameter
        {
            // this size specifies how large the render target should be in pixels
            Size render_target;

            std::shared_ptr<ParticleCollection> collection = nullptr;
            float rest_density = 0.0f;
            float particle_size = 0.0f;

            NeighborhoodInterface* neighborhood_interface = nullptr;

        } parameters;

        virtual void initialize() = 0;

        virtual void render() = 0;

        virtual Image get_image_data() = 0;

        virtual ~ISimulationVisualizer() = default;
    };
} // namespace FluidSolver

#endif // FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
