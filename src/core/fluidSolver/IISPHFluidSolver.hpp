//
// Created by corbi on 20.11.2019.
//

#ifndef FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_IISPHFLUIDSOLVER_HPP

#include <core/fluidSolver/IFluidSolver.hpp>

namespace FluidSolver {
    class IISPHFluidSolver : public IFluidSolver {

    private:
        float Timestep = 0.0f;
        float RestDensity = 0.0f;
        float ParticleSize = 0.0f;

        IParticleCollection* ParticleCollection = nullptr;

    public:
        void ExecuteSimulationStep() override;

        float getParticleSize() override;

        void setParticleSize(float particleSize) override;

        float getRestDensity() override;

        void setRestDensity(float restDensity) override;

        float getTimestep() override;

        void setTimestep(float timestep) override;

        void setParticleCollection(IParticleCollection *particleCollection) override;

        IParticleCollection *getParticleCollection() override;
    };
}


#endif //FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
