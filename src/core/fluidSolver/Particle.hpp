#ifndef FLUIDSOLVER_PARTICLE_HPP
#define FLUIDSOLVER_PARTICLE_HPP


#include <engine/libraries/glm/glm.hpp>

namespace FluidSolver {

    enum ParticleType {
        ParticleTypeNormal = 0,
        ParticleTypeBoundary = 1,
        ParticleTypeDead = 2,
    };

    struct MovementData {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 acceleration;
    };

    struct ParticleData {
        float mass;
        float pressure;
        float density;
    };

    struct ParticleInfo {
        uint32_t tag;
        uint8_t type;
    };

    struct ExternalForces{
        glm::vec2 non_pressure_acceleration;
    };

}

#endif //FLUIDSOLVER_PARTICLE_HPP
