//
// Created by gruberc on 24.04.19.
//

#ifndef FLUIDSOLVER_IPARTICLECOLLECTION_HPP
#define FLUIDSOLVER_IPARTICLECOLLECTION_HPP

#include <glm/glm.hpp>

class IParticleCollection {

public:
    virtual float GetMass(uint32_t index) = 0;

    virtual void SetMass(uint32_t index, float value) = 0;

    virtual float GetPressure(uint32_t index) = 0;

    virtual void SetPressure(uint32_t index, float value) = 0;

    virtual glm::vec3 GetPosition(uint32_t index) = 0;

    virtual void SetPosition(uint32_t index, glm::vec3 value) = 0;

    virtual glm::vec3 GetVelocity(uint32_t index) = 0;

    virtual void SetVelocity(uint32_t index, glm::vec3 value) = 0;

    virtual glm::vec3 GetAcceleration(uint32_t index) = 0;

    virtual void SetAcceleration(uint32_t index, glm::vec3 value) = 0;
};


#endif //FLUIDSOLVER_IPARTICLECOLLECTION_HPP
