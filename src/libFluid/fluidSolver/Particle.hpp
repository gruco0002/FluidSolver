#pragma once

#include "FluidInclude.hpp"

namespace LibFluid {

    enum ParticleType
    {
        ParticleTypeNormal = 0,
        ParticleTypeBoundary = 1,
        ParticleTypeDead = 2,
    };


    struct MovementData
    {
        vec2 position;
        vec2 velocity;
        vec2 acceleration;
    };

    struct MovementData3D
    {
        vec3 position;
        vec3 velocity;
        vec3 acceleration;
    };

    struct ParticleData
    {
        pFloat mass;
        pFloat pressure;
        pFloat density;
    };

    struct ParticleInfo
    {
        pTag_t tag;
        uint8_t type;
    };

    struct ExternalForces
    {
        vec2 non_pressure_acceleration;
    };

    struct ExternalForces3D
    {
        vec3 non_pressure_acceleration;
    };

} // namespace FluidSolver
