//
// Created by corbi on 20.11.2019.
//

#ifndef FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
#define FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP

#include "IParticleCollection.hpp"

namespace FluidSolver {
    class ISimulationVisualizer {

    public:
        IParticleCollection *particleCollection = nullptr;

        virtual void Render() = 0;

    };
}

#endif //FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
