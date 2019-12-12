//
// Created by Corbinian Gruber on 12.12.19.
//

#ifndef FLUIDSOLVER_CONSTANTTIMESTEP_HPP
#define FLUIDSOLVER_CONSTANTTIMESTEP_HPP

#include "ITimestep.hpp"

namespace FluidSolver {
    class ConstantTimestep : public ITimestep {
    private:
        float timestep = 0.001f;
    public:
        float getTimestep() const;

        void setTimestep(float timestep);

        void CalculateCurrentTimestep() override;

        float getCurrentTimestep() override;

        explicit ConstantTimestep(float timestep);


    };
}


#endif //FLUIDSOLVER_CONSTANTTIMESTEP_HPP
