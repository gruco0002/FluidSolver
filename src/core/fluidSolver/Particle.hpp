#ifndef FLUIDSOLVER_PARTICLE_HPP
#define FLUIDSOLVER_PARTICLE_HPP


#include <core/FluidInclude.hpp>

namespace FluidSolver {

    enum ParticleType {
        ParticleTypeNormal = 0,
        ParticleTypeBoundary = 1,
        ParticleTypeDead = 2,
    };

    struct MovementData {
        vec2 position;
        vec2 velocity;
        vec2 acceleration;
    };

    struct ParticleData {
        pFloat mass;
        pFloat pressure;
        pFloat density;
    };

    struct ParticleInfo {
        pTag_t tag;
        uint8_t type;
    };

    struct ExternalForces {
        vec2 non_pressure_acceleration;
    };

}

#endif //FLUIDSOLVER_PARTICLE_HPP
