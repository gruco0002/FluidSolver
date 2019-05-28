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

        /**
         * Calculates the average density. Boundary Particles and Particles with Density Below the Rest Density are ignored.
         * @return
         */
        virtual float CalculateAverageDensity(float restDensity) = 0;

        /**
         * Calculates the current Energy of all particles except Boundary Particles.
         * @param zeroHeight The zero Height used for potential energy calculation.
         * @param gravity Used for potential energy calculation.
         * @return
         */
        virtual float CalculateEnergy(float zeroHeight, float gravity) = 0;

        /**
         * Calculates the maximal velocity of all particles.
         * @return
         */
        virtual float CalculateMaximumVelocity() = 0;


    };

}
#endif //FLUIDSOLVER_IPARTICLECOLLECTION_HPP
