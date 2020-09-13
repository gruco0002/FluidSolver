#ifndef FLUIDSOLVER_SCENARIODATA_HPP
#define FLUIDSOLVER_SCENARIODATA_HPP

#include "../fluidSolver/ParticleCollection.hpp"
namespace FluidSolver {
    struct ScenarioData {

        ParticleCollection collection;

        float particle_size = 1.0f;

        float rest_density = 1.0f;

    };

}
#endif //FLUIDSOLVER_SCENARIODATA_HPP
