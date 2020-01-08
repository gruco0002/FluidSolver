//
// Created by gruberc on 24.04.19.
//

#ifndef FLUIDSOLVER_SIMPLEPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_SIMPLEPARTICLECOLLECTION_HPP

#include "libraries/glm/glm.hpp"
#include "IParticleCollection.hpp"
#include <vector>

namespace FluidSolver {
    class SimpleParticleCollection : public IParticleCollection {


    public:

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

        SimpleParticleCollection(std::vector<FluidParticle> &input);


        float GetMass(uint32_t index) override;

        void SetMass(uint32_t index, float value) override;

        float GetPressure(uint32_t index) override;

        void SetPressure(uint32_t index, float value) override;

        glm::vec2 GetPosition(uint32_t index) override;

        void SetPosition(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetNonPressureAcceleration(uint32_t index) override;

        void SetNonPressureAcceleration(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetPredictedVelocity(uint32_t index) override;

        void SetPredictedVelocity(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetVelocity(uint32_t index) override;

        void SetVelocity(uint32_t index, glm::vec2 value) override;

        glm::vec2 GetAcceleration(uint32_t index) override;

        void SetAcceleration(uint32_t index, glm::vec2 value) override;

        uint32_t GetSize() override;

        float GetDensity(uint32_t index) override;

        void SetDensity(uint32_t index, float value) override;

        ParticleType GetParticleType(uint32_t index) override;

        void SetParticleType(uint32_t index, ParticleType value) override;

        std::vector<FluidParticle> &GetParticles();

        void SetParticles(std::vector<FluidParticle> &input);

        float GetSourceTerm(uint32_t index) override;

        void SetSourceTerm(uint32_t index, float value) override;

        float GetDiagonalElement(uint32_t index) override;

        void SetDiagonalElement(uint32_t index, float value) override;


    private:


        std::vector<FluidParticle> particles;

    };

}
#endif //FLUIDSOLVER_SIMPLEPARTICLECOLLECTION_HPP
