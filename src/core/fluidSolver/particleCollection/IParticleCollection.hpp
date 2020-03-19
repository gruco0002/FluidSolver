//
// Created by gruberc on 24.04.19.
//

#ifndef FLUIDSOLVER_IPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_IPARTICLECOLLECTION_HPP

#include "libraries/glm/glm.hpp"

namespace FluidSolver {

    enum ParticleType {
        ParticleTypeNormal = 0,
        ParticleTypeBoundary = 1,
        ParticleTypeDead = 2,
    };


    struct FluidParticle {
        glm::vec2 Position;
        glm::vec2 Velocity;
        glm::vec2 Acceleration;

        float Mass;
        float Pressure;
        float Density;

        uint8_t Type;
        glm::vec2 PredictedVelocity = glm::vec2(0.0f);
        glm::vec2 NonPressureAcceleration = glm::vec2(0.0f);
        float SourceTerm = 0.0f;
        float DiagonalElement = 0.0f;

        bool operator==(const FluidParticle &rhs) const;

        bool operator!=(const FluidParticle &rhs) const;


    };

    class IParticleCollection {

    public:



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

        virtual glm::vec2 GetNonPressureAcceleration(uint32_t index) = 0;

        virtual void SetNonPressureAcceleration(uint32_t index, glm::vec2 value) = 0;

        virtual glm::vec2 GetPredictedVelocity(uint32_t index) = 0;

        virtual void SetPredictedVelocity(uint32_t index, glm::vec2 value) = 0;

        virtual float GetSourceTerm(uint32_t index) = 0;

        virtual void SetSourceTerm(uint32_t index, float value) = 0;

        virtual float GetDiagonalElement(uint32_t index) = 0;

        virtual void SetDiagonalElement(uint32_t index, float value) = 0;

        virtual ~IParticleCollection() = default;


    };

}
#endif //FLUIDSOLVER_IPARTICLECOLLECTION_HPP
