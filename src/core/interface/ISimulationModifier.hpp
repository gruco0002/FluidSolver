//
// Created by corbi on 06.06.2019.
//

#ifndef FLUIDSOLVER_ISIMULATIONMODIFIER_HPP
#define FLUIDSOLVER_ISIMULATIONMODIFIER_HPP

#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"

namespace FluidSolver{

    /**
     * An interface providing Simulation modification.
     */
    class ISimulationModifier{


    public:

        struct SimulationInfo{
            float timeStep;
            float restDensity;
            float particleSize;
        };

        /**
         * This function will be called after advecting all particles.
         * @param collection The particle Collection.
         */
        virtual void ModifySimulation(IParticleCollection* collection, SimulationInfo &info) = 0;


    };

}

#endif //FLUIDSOLVER_ISIMULATIONMODIFIER_HPP
