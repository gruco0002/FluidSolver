//
// Created by corbi on 18.12.2019.
//

#ifndef FLUIDSOLVER_CONTINOUSVISUALIZER_HPP
#define FLUIDSOLVER_CONTINOUSVISUALIZER_HPP

#include <core/interface/ISimulationVisualizer.hpp>

namespace FluidSolver {
    class ContinousVisualizer : public ISimulationVisualizer {
    public:
        void setParticleCollection(IParticleCollection *particleCollection) override;

        IParticleCollection *getParticleCollection() override;

        void setParticleSize(float particleSize) override;

        float getParticleSize() override;

        void Render() override;

        float getRestDensity() override;

        void setRestDensity(float restDensity) override;

    private:

        float ParticleSize = 0.0f;
        IParticleCollection* ParticleCollection = nullptr;
        float RestDensity = 0.0f;


    };
}


#endif //FLUIDSOLVER_CONTINOUSVISUALIZER_HPP
