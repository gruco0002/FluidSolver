//
// Created by gruberc on 24.04.19.
//

#include <algorithm>
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
        SetParticles(input);
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

    void SimpleParticleCollection::SetParticles(std::vector<FluidParticle> &input) {
        particles = input;
    }

    float SimpleParticleCollection::CalculateAverageDensity(float restDensity) {

        double densitySum = 0;
        uint32_t sumCount = 0;

        for (SimpleParticleCollection::FluidParticle &p : particles) {
            if (p.Type == ParticleTypeBoundary)
                continue;
            if (p.Density < restDensity)
                continue;
            densitySum += p.Density;
            sumCount++;
        }

        return densitySum / (double) sumCount;
    }

    float SimpleParticleCollection::CalculateEnergy(float zeroHeight, float gravity) {
        float energySum = 0.0f;
        for (SimpleParticleCollection::FluidParticle &p : particles) {
            if (p.Type == ParticleTypeBoundary)
                continue;

            float particleVelocity = glm::length(p.Velocity);

            // potential energy
            energySum += (p.Position.y - zeroHeight) * p.Mass * gravity;

            // kinetic energy
            energySum += 0.5f * p.Mass * particleVelocity * particleVelocity;

        }

        return energySum;
    }

    float SimpleParticleCollection::CalculateMaximumVelocity() {
        if (GetSize() == 0)return 0;
        float maximum = glm::length(particles[0].Velocity);

        for (SimpleParticleCollection::FluidParticle &p : particles) {
            maximum = std::max(maximum, glm::length(p.Velocity));
        }
        return maximum;
    }

    bool SimpleParticleCollection::FluidParticle::operator==(const SimpleParticleCollection::FluidParticle &rhs) const {
        return Position == rhs.Position &&
               Velocity == rhs.Velocity &&
               Acceleration == rhs.Acceleration &&
               Mass == rhs.Mass &&
               Pressure == rhs.Pressure &&
               Density == rhs.Density &&
               Type == rhs.Type;
    }

    bool SimpleParticleCollection::FluidParticle::operator!=(const SimpleParticleCollection::FluidParticle &rhs) const {
        return !(rhs == *this);
    }
}