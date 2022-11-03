#pragma once

#include <glm/glm.hpp>

namespace LibFluid {

    enum ParticleType
    {
        ParticleTypeNormal = 0,
        ParticleTypeBoundary = 1,
        ParticleTypeInactive = 2,
    };


    struct MovementData
    {
        glm::vec2 position;
        glm::vec2 velocity;
        glm::vec2 acceleration;
    };

    struct MovementData3D
    {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;
    };

    struct ParticleData
    {
        float mass;
        float pressure;
        float density;
    };

    struct ParticleInfo
    {
        uint32_t tag;
        uint8_t type;
    };

    struct ExternalForces
    {
        glm::vec2 non_pressure_acceleration;
    };

    struct ExternalForces3D
    {
        glm::vec3 non_pressure_acceleration;
    };

} // namespace FluidSolver
