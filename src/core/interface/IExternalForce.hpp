#ifndef FLUIDSOLVER_IEXTERNALFORCE_HPP
#define FLUIDSOLVER_IEXTERNALFORCE_HPP

namespace FluidSolver {
    class IExternalForce {

    public:
        virtual void Apply(IParticleCollection *particleCollection) = 0;

    };

}

#endif //FLUIDSOLVER_IEXTERNALFORCE_HPP
