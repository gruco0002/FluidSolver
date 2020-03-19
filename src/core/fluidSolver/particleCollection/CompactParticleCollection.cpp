
#include "CompactParticleCollection.hpp"

size_t FluidSolver::CompactParticleCollection::GetParticleID(size_t index) {
    return particleIndexToParticleIDMap[index];
}

size_t FluidSolver::CompactParticleCollection::GetIndex(size_t particleID) {
    return particleIDToParticleIndexMap[particleID];
}

size_t FluidSolver::CompactParticleCollection::AddParticle(const FluidSolver::FluidParticle &fluidParticle) {

    auto index = AddEmptyParticle();

    particleData[index] = fluidParticle;

    return index;
}

size_t FluidSolver::CompactParticleCollection::AddEmptyParticle() {
    // get the new particle index
    size_t index = particleData.size();

    // create empty particle data
    particleData.push_back(FluidParticle());

    // set the index mapping data
    particleIDToParticleIndexMap.push_back(index);
    particleIndexToParticleIDMap.push_back(index);
    sortKeys.push_back(0);

    return index;
}

float FluidSolver::CompactParticleCollection::GetMass(uint32_t index) {
    return particleData[index].Mass;
}

void FluidSolver::CompactParticleCollection::SetMass(uint32_t index, float value) {
    particleData[index].Mass = value;
}

float FluidSolver::CompactParticleCollection::GetPressure(uint32_t index) {
    return particleData[index].Pressure;
}

void FluidSolver::CompactParticleCollection::SetPressure(uint32_t index, float value) {
    particleData[index].Pressure = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetPosition(uint32_t index) {
    return particleData[index].Position;
}

void FluidSolver::CompactParticleCollection::SetPosition(uint32_t index, glm::vec2 value) {
    particleData[index].Position = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetVelocity(uint32_t index) {
    return particleData[index].Velocity;
}

void FluidSolver::CompactParticleCollection::SetVelocity(uint32_t index, glm::vec2 value) {
    particleData[index].Velocity = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetAcceleration(uint32_t index) {
    return particleData[index].Acceleration;
}

void FluidSolver::CompactParticleCollection::SetAcceleration(uint32_t index, glm::vec2 value) {
    particleData[index].Acceleration = value;
}

uint32_t FluidSolver::CompactParticleCollection::GetSize() {
    return particleData.size();
}

float FluidSolver::CompactParticleCollection::GetDensity(uint32_t index) {
    return particleData[index].Density;
}

void FluidSolver::CompactParticleCollection::SetDensity(uint32_t index, float value) {
    particleData[index].Density = value;
}

FluidSolver::ParticleType FluidSolver::CompactParticleCollection::GetParticleType(uint32_t index) {
    return (FluidSolver::ParticleType) particleData[index].Type;
}

void FluidSolver::CompactParticleCollection::SetParticleType(uint32_t index, FluidSolver::ParticleType value) {
    particleData[index].Type = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetNonPressureAcceleration(uint32_t index) {
    return particleData[index].NonPressureAcceleration;
}

void FluidSolver::CompactParticleCollection::SetNonPressureAcceleration(uint32_t index, glm::vec2 value) {
    particleData[index].NonPressureAcceleration = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetPredictedVelocity(uint32_t index) {
    return particleData[index].PredictedVelocity;
}

void FluidSolver::CompactParticleCollection::SetPredictedVelocity(uint32_t index, glm::vec2 value) {
    particleData[index].PredictedVelocity = value;
}

float FluidSolver::CompactParticleCollection::GetSourceTerm(uint32_t index) {
    return particleData[index].SourceTerm;
}

void FluidSolver::CompactParticleCollection::SetSourceTerm(uint32_t index, float value) {
    particleData[index].SourceTerm = value;
}

float FluidSolver::CompactParticleCollection::GetDiagonalElement(uint32_t index) {
    return particleData[index].DiagonalElement;
}

void FluidSolver::CompactParticleCollection::SetDiagonalElement(uint32_t index, float value) {
    particleData[index].DiagonalElement = value;
}

uint64_t FluidSolver::CompactParticleCollection::GetSortKey(size_t index) {
    return sortKeys[index];
}

void FluidSolver::CompactParticleCollection::PrecalculateSortKeys(
        const FluidSolver::IParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // TODO: parallelization
    for (size_t i = 0; i < GetSize(); i++) {
        sortKeys[i] = sortKeyFunction(i);
    }
}

void FluidSolver::CompactParticleCollection::SwapElements(size_t i, size_t j) {
    std::swap(sortKeys[i], sortKeys[j]);
    std::swap(particleIndexToParticleIDMap[i], particleIndexToParticleIDMap[j]);
    std::swap(particleIDToParticleIndexMap[i], particleIDToParticleIndexMap[j]);

    std::swap(particleData[i], particleData[j]);
}

FluidSolver::FluidParticle *FluidSolver::CompactParticleCollection::GetData() {
    return particleData.data();
}
