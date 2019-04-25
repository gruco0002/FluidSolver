//
// Created by gruberc on 24.04.19.
//

#include "SimpleParticleCollection.hpp"

namespace FluidSolver {
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

    glm::vec2 SimpleParticleCollection::GetPosition(uint32_t index) {
        return particles[index].Position;
    }

    void SimpleParticleCollection::SetPosition(uint32_t index, glm::vec2 value) {
        particles[index].Position = value;
    }

    glm::vec2 SimpleParticleCollection::GetVelocity(uint32_t index) {
        return particles[index].Velocity;
    }

    void SimpleParticleCollection::SetVelocity(uint32_t index, glm::vec2 value) {
        particles[index].Velocity = value;
    }

    glm::vec2 SimpleParticleCollection::GetAcceleration(uint32_t index) {
        return particles[index].Acceleration;
    }

    void SimpleParticleCollection::SetAcceleration(uint32_t index, glm::vec2 value) {
        particles[index].Acceleration = value;
    }

    SimpleParticleCollection::SimpleParticleCollection(std::vector<SimpleParticleCollection::FluidParticle> &input) {
        particles = input;
    }

    uint32_t SimpleParticleCollection::GetSize() {
        return particles.size();
    }

    float SimpleParticleCollection::GetDensity(uint32_t index) {
        return particles[index].Density;
    }

    void SimpleParticleCollection::SetDensity(uint32_t index, float value) {
        particles[index].Density = value;
    }

    std::vector<SimpleParticleCollection::FluidParticle> &SimpleParticleCollection::GetParticles() {
        return particles;
    }

    IParticleCollection::ParticleType SimpleParticleCollection::GetParticleType(uint32_t index) {
        return (ParticleType) particles[index].Type;
    }

    void SimpleParticleCollection::SetParticleType(uint32_t index, IParticleCollection::ParticleType value) {
        particles[index].Type = value;
    }
}