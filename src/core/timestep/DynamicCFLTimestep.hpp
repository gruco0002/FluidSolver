//
// Created by Corbinian Gruber on 12.12.19.
//

#ifndef FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP
#define FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP

#include "ITimestep.hpp"

namespace FluidSolver {
    class DynamicCFLTimestep : public ITimestep {

    private:
        float currentTimestep = 0.0f;

        float maximumTimestep = 0.1f;
        float minimumTimestep = 0.0001f;
        float CFLNumber = 0.4f;

        float CalculateMaximumVelocity();

    public:
        void CalculateCurrentTimestep() override;

        float getCurrentTimestep() override;

    };
}


#endif //FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP
