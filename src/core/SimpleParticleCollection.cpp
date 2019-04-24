//
// Created by gruberc on 24.04.19.
//

#include "ParticleCollection.hpp"

float ParticleCollection::GetMass(uint32_t index) {
    return particles[index].Mass;
}

void ParticleCollection::SetMass(uint32_t index, float value) {
    particles[index].Mass = value;
}

float ParticleCollection::GetPressure(uint32_t index) {
    return particles[index].Pressure;
}

void ParticleCollection::SetPressure(uint32_t index, float value) {
    particles[index].Pressure = value;
}

glm::vec3 ParticleCollection::GetPosition(uint32_t index) {
    return particles[index].Position;
}

void ParticleCollection::SetPosition(uint32_t index, glm::vec3 value) {
    particles[index].Position = value;
}

glm::vec3 ParticleCollection::GetVelocity(uint32_t index) {
    return particles[index].Velocity;
}

void ParticleCollection::SetVelocity(uint32_t index, glm::vec3 value) {
    particles[index].Velocity = value;
}

glm::vec3 ParticleCollection::GetAcceleration(uint32_t index) {
    return particles[index].Acceleration;
}

void ParticleCollection::SetAcceleration(uint32_t index, glm::vec3 value) {
    particles[index].Acceleration = value;
}
