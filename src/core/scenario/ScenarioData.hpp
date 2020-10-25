#ifndef FLUIDSOLVER_SCENARIODATA_HPP
#define FLUIDSOLVER_SCENARIODATA_HPP

#include "../fluidSolver/ParticleCollection.hpp"
#include <core/entities/IEntity.hpp>

namespace FluidSolver {
    struct ScenarioData {

        ParticleCollection collection;

        float particle_size = 1.0f;

        float rest_density = 1.0f;

        std::string name;

        struct Viewport {
            float left;
            float right;
            float top;
            float bottom;
        } viewport;

        std::vector<IEntity *> entities;

        ~ScenarioData();

    };

}
#endif //FLUIDSOLVER_SCENARIODATA_HPP
