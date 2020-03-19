#ifndef FLUIDSOLVER_CONSTANTTIMESTEP_HPP
#define FLUIDSOLVER_CONSTANTTIMESTEP_HPP

#include "ITimestep.hpp"

namespace FluidSolver {
    class ConstantTimestep : public ITimestep {


    public:
        float Timestep = 0.001f;

        float getTimestep() const;

        void setTimestep(float timestep);

        void CalculateCurrentTimestep() override;

        float getCurrentTimestep() override;

        explicit ConstantTimestep(float timestep);


    };
}


#endif //FLUIDSOLVER_CONSTANTTIMESTEP_HPP
