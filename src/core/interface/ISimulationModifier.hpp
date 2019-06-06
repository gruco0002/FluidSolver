//
// Created by corbi on 06.06.2019.
//

#ifndef FLUIDSOLVER_ISIMULATIONMODIFIER_HPP
#define FLUIDSOLVER_ISIMULATIONMODIFIER_HPP

#include "IParticleCollection.hpp"

namespace FluidSolver{

    /**
     * An interface providing Simulation modification.
     */
    class ISimulationModifier{


    public:

        /**
         * This function will be called after advecting all particles.
         * @param collection The particle Collection.
         */
        virtual void ModifySimulation(IParticleCollection* collection) = 0;


    };

}

#endif //FLUIDSOLVER_ISIMULATIONMODIFIER_HPP
