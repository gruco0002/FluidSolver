#ifndef FLUIDSOLVER_SCRIPTINTERFACE_HPP
#define FLUIDSOLVER_SCRIPTINTERFACE_HPP

#include "ScenarioData.hpp"
#include "chaiscript/chaiscript.hpp"

namespace FluidSolver {
    struct ScriptInterface {

        struct Particle {
            float x = 0.0f;
            float y = 0.0f;
            uint8_t type = ParticleType::ParticleTypeNormal;
            float density;
            float mass;
        };


        ScenarioData *current = nullptr;

        pIndex_t add_particle(Particle particle);

        void make_available(chaiscript::ChaiScript &chai);

    };

}

#endif //FLUIDSOLVER_SCRIPTINTERFACE_HPP
