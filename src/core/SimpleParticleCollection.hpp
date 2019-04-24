//
// Created by gruberc on 24.04.19.
//

#ifndef FLUIDSOLVER_PARTICLECOLLECTION_HPP
#define FLUIDSOLVER_PARTICLECOLLECTION_HPP

#include "glm/glm.hpp"
#include "IParticleCollection.hpp"
#include <vector>

class ParticleCollection : public IParticleCollection {


public:

    struct FluidParticle {
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec3 Acceleration;

        float Mass;
        float Pressure;
    };


    float GetMass(uint32_t index) override;

    void SetMass(uint32_t index, float value) override;

    float GetPressure(uint32_t index) override;

    void SetPressure(uint32_t index, float value) override;

    glm::vec3 GetPosition(uint32_t index) override;

    void SetPosition(uint32_t index, glm::vec3 value) override;

    glm::vec3 GetVelocity(uint32_t index) override;

    void SetVelocity(uint32_t index, glm::vec3 value) override;

    glm::vec3 GetAcceleration(uint32_t index) override;

    void SetAcceleration(uint32_t index, glm::vec3 value) override;

private:


    std::vector<FluidParticle> particles;

};


#endif //FLUIDSOLVER_PARTICLECOLLECTION_HPP
