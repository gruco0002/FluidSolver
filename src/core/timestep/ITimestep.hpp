#ifndef FLUIDSOLVER_ITIMESTEP_HPP
#define FLUIDSOLVER_ITIMESTEP_HPP

#include <core/fluidSolver/ParticleCollection.hpp>

namespace FluidSolver {
    class ITimestep {

    protected:
        ParticleCollection *particleCollection = nullptr;

        float ParticleSize = 1.0f;
    public:
        float getParticleSize() const;

        void setParticleSize(float particleSize);

    public:

        virtual ParticleCollection *getParticleCollection();

        virtual void setParticleCollection(ParticleCollection *particleCollection);

        virtual void CalculateCurrentTimestep() = 0;

        virtual float getCurrentTimestep() = 0;

        virtual ~ITimestep();
    };
}


#endif //FLUIDSOLVER_ITIMESTEP_HPP
