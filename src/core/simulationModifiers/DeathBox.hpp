#ifndef FLUIDSOLVER_DEATHBOX_HPP
#define FLUIDSOLVER_DEATHBOX_HPP

#include <core/interface/ISimulationModifier.hpp>

namespace FluidSolver {
    /**
     * Sets the type of every particle outside or inside the box to dead.
     */
    class DeathBox : public ISimulationModifier {
    public:
        DeathBox(float top, float left, float bottom, float right);

        // box dimensions
        float top;
        float left;
        float bottom;
        float right;

        /**
         * If set to true, particles outside will be killed. Otherwise particles on the inside will be killed.
         */
        bool outside = true;

        void ModifySimulation(IParticleCollection *collection, SimulationInfo &info) override;


    };
}

#endif //FLUIDSOLVER_DEATHBOX_HPP
