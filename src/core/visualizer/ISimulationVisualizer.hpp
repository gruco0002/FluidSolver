#ifndef FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
#define FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP

// #include <core/selection/IParticleSelection.hpp>
#include "core/fluidSolver/ParticleCollection.hpp"

namespace FluidSolver {
    class ISimulationVisualizer {

    public:
        /**
         * This defines a view area in simulation space. The coordinates are relative to
         * simulation space origin.
         */
        struct SimulationViewArea {
            float Left = 0.0f;
            float Top = 0.0f;

            float Right = 0.0f;
            float Bottom = 0.0f;
        };

    public:
        virtual void setParticleCollection(ParticleCollection *particleCollection) = 0;

        virtual ParticleCollection *getParticleCollection() = 0;

        virtual void setParticleSize(float particleSize) = 0;

        virtual float getParticleSize() = 0;

        virtual float getRestDensity() = 0;

        virtual void setRestDensity(float restDensity) = 0;

        virtual void UpdateData() = 0;

        virtual void Render() = 0;

        virtual void setSimulationViewArea(SimulationViewArea viewArea) = 0;

        virtual void setRenderTargetSize(size_t width, size_t height) = 0;

        virtual ~ISimulationVisualizer() = default;

        virtual glm::vec2 ConvertPixelCoordinateToParticleSpace(size_t pixelX, size_t pixelY) = 0;

        //virtual void setParticleSelection(IParticleSelection *particleSelection) = 0;

        //virtual IParticleSelection *getParticleSelection() = 0;

    };
}

#endif //FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
