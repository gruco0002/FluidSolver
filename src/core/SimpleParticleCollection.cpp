//
// Created by gruberc on 24.04.19.
//

#include "SimpleParticleCollection.hpp"

float SimpleParticleCollection::GetMass(uint32_t index) {
    return particles[index].Mass;
}

void SimpleParticleCollection::SetMass(uint32_t index, float value) {
    particles[index].Mass = value;
}

float SimpleParticleCollection::GetPressure(uint32_t index) {
    return particles[index].Pressure;
}

void SimpleParticleCollection::SetPressure(uint32_t index, float value) {
    particles[index].Pressure = value;
}

glm::vec3 SimpleParticleCollection::GetPosition(uint32_t index) {
    return particles[index].Position;
}

void SimpleParticleCollection::SetPosition(uint32_t index, glm::vec3 value) {
    particles[index].Position = value;
}

glm::vec3 SimpleParticleCollection::GetVelocity(uint32_t index) {
    return particles[index].Velocity;
}

void SimpleParticleCollection::SetVelocity(uint32_t index, glm::vec3 value) {
    particles[index].Velocity = value;
}

glm::vec3 SimpleParticleCollection::GetAcceleration(uint32_t index) {
    return particles[index].Acceleration;
}

void SimpleParticleCollection::SetAcceleration(uint32_t index, glm::vec3 value) {
    particles[index].Acceleration = value;
}
