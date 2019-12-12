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

        float MaximumTimestep = 0.1f;
        float MinimumTimestep = 0.0001f;
        float CFLNumber = 0.4f;
    public:
        DynamicCFLTimestep(float maximumTimestep, float minimumTimestep, float cflNumber);

        explicit DynamicCFLTimestep();

        float getMaximumTimestep() const;

        void setMaximumTimestep(float maximumTimestep);

        float getMinimumTimestep() const;

        void setMinimumTimestep(float minimumTimestep);

        float getCflNumber() const;

        void setCflNumber(float cflNumber);

    private:

        float CalculateMaximumVelocity();

    public:
        void CalculateCurrentTimestep() override;

        float getCurrentTimestep() override;

    };
}


#endif //FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP
