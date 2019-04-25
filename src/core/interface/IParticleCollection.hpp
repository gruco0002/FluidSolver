//
// Created by gruberc on 24.04.19.
//

#ifndef FLUIDSOLVER_IPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_IPARTICLECOLLECTION_HPP

#include "libraries/glm/glm.hpp"

namespace FluidSolver {
    class IParticleCollection {

    public:
        enum ParticleType {
            ParticleTypeNormal = 0,
            ParticleTypeBoundary = 1,
        };


        virtual float GetMass(uint32_t index) = 0;

        virtual void SetMass(uint32_t index, float value) = 0;

        virtual float GetPressure(uint32_t index) = 0;

        virtual void SetPressure(uint32_t index, float value) = 0;

        virtual glm::vec2 GetPosition(uint32_t index) = 0;

        virtual void SetPosition(uint32_t index, glm::vec2 value) = 0;

        virtual glm::vec2 GetVelocity(uint32_t index) = 0;

        virtual void SetVelocity(uint32_t index, glm::vec2 value) = 0;

        virtual glm::vec2 GetAcceleration(uint32_t index) = 0;

        virtual void SetAcceleration(uint32_t index, glm::vec2 value) = 0;

        virtual uint32_t GetSize() = 0;

        virtual float GetDensity(uint32_t index) = 0;

        virtual void SetDensity(uint32_t index, float value) = 0;

        virtual ParticleType GetParticleType(uint32_t index) = 0;

        virtual void SetParticleType(uint32_t index, ParticleType value) = 0;

    };

}
#endif //FLUIDSOLVER_IPARTICLECOLLECTION_HPP
