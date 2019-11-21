//
// Created by corbi on 20.11.2019.
//

#ifndef FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
#define FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP

#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"

namespace FluidSolver {
    class ISimulationVisualizer {

    public:
        virtual void setParticleCollection(IParticleCollection *particleCollection) = 0;

        virtual IParticleCollection *getParticleCollection() = 0;

        virtual void setParticleSize(float particleSize) = 0;

        virtual float getParticleSize() = 0;

        virtual void Render() = 0;

    };
}

#endif //FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
