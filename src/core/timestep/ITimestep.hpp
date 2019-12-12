//
// Created by Corbinian Gruber on 12.12.19.
//

#ifndef FLUIDSOLVER_ITIMESTEP_HPP
#define FLUIDSOLVER_ITIMESTEP_HPP

#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>

namespace FluidSolver {
    class ITimestep {

    protected:
        IParticleCollection *particleCollection = nullptr;
    public:

        virtual IParticleCollection *getParticleCollection();

        virtual void setParticleCollection(IParticleCollection *particleCollection);

        virtual void CalculateCurrentTimestep() = 0;

        virtual float getCurrentTimestep() = 0;

        virtual ~ITimestep();
    };
}


#endif //FLUIDSOLVER_ITIMESTEP_HPP
