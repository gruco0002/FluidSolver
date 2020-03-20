
#include "CompactParticleCollection.hpp"

FluidSolver::particleID_t FluidSolver::CompactParticleCollection::GetParticleID(particleIndex_t index) {
    return particleIndexToParticleIDMap[index];
}

FluidSolver::particleIndex_t FluidSolver::CompactParticleCollection::GetIndex(particleID_t particleID) {
    return particleIDToParticleIndexMap[particleID];
}

FluidSolver::particleIndex_t
FluidSolver::CompactParticleCollection::AddParticle(const FluidSolver::FluidParticle &fluidParticle) {

    auto index = AddEmptyParticle();

    particleData[index] = fluidParticle;

    return index;
}

FluidSolver::particleIndex_t FluidSolver::CompactParticleCollection::AddEmptyParticle() {
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

float FluidSolver::CompactParticleCollection::GetMass(particleIndex_t index) {
    return particleData[index].Mass;
}

void FluidSolver::CompactParticleCollection::SetMass(particleIndex_t index, float value) {
    particleData[index].Mass = value;
}

float FluidSolver::CompactParticleCollection::GetPressure(particleIndex_t index) {
    return particleData[index].Pressure;
}

void FluidSolver::CompactParticleCollection::SetPressure(particleIndex_t index, float value) {
    particleData[index].Pressure = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetPosition(particleIndex_t index) {
    return particleData[index].Position;
}

void FluidSolver::CompactParticleCollection::SetPosition(particleIndex_t index, glm::vec2 value) {
    particleData[index].Position = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetVelocity(particleIndex_t index) {
    return particleData[index].Velocity;
}

void FluidSolver::CompactParticleCollection::SetVelocity(particleIndex_t index, glm::vec2 value) {
    particleData[index].Velocity = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetAcceleration(particleIndex_t index) {
    return particleData[index].Acceleration;
}

void FluidSolver::CompactParticleCollection::SetAcceleration(particleIndex_t index, glm::vec2 value) {
    particleData[index].Acceleration = value;
}

FluidSolver::particleAmount_t FluidSolver::CompactParticleCollection::GetSize() {
    return particleData.size();
}

float FluidSolver::CompactParticleCollection::GetDensity(particleIndex_t index) {
    return particleData[index].Density;
}

void FluidSolver::CompactParticleCollection::SetDensity(particleIndex_t index, float value) {
    particleData[index].Density = value;
}

FluidSolver::ParticleType FluidSolver::CompactParticleCollection::GetParticleType(particleIndex_t index) {
    return (FluidSolver::ParticleType) particleData[index].Type;
}

void FluidSolver::CompactParticleCollection::SetParticleType(particleIndex_t index, FluidSolver::ParticleType value) {
    particleData[index].Type = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetNonPressureAcceleration(particleIndex_t index) {
    return particleData[index].NonPressureAcceleration;
}

void FluidSolver::CompactParticleCollection::SetNonPressureAcceleration(particleIndex_t index, glm::vec2 value) {
    particleData[index].NonPressureAcceleration = value;
}

glm::vec2 FluidSolver::CompactParticleCollection::GetPredictedVelocity(particleIndex_t index) {
    return particleData[index].PredictedVelocity;
}

void FluidSolver::CompactParticleCollection::SetPredictedVelocity(particleIndex_t index, glm::vec2 value) {
    particleData[index].PredictedVelocity = value;
}

float FluidSolver::CompactParticleCollection::GetSourceTerm(particleIndex_t index) {
    return particleData[index].SourceTerm;
}

void FluidSolver::CompactParticleCollection::SetSourceTerm(particleIndex_t index, float value) {
    particleData[index].SourceTerm = value;
}

float FluidSolver::CompactParticleCollection::GetDiagonalElement(particleIndex_t index) {
    return particleData[index].DiagonalElement;
}

void FluidSolver::CompactParticleCollection::SetDiagonalElement(particleIndex_t index, float value) {
    particleData[index].DiagonalElement = value;
}

uint64_t FluidSolver::CompactParticleCollection::GetSortKey(particleIndex_t index) {
    return sortKeys[index];
}

void FluidSolver::CompactParticleCollection::PrecalculateSortKeys(
        const FluidSolver::IParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // TODO: parallelization
    for (size_t i = 0; i < GetSize(); i++) {
        sortKeys[i] = sortKeyFunction(i);
    }
}

void FluidSolver::CompactParticleCollection::SwapElements(particleIndex_t i, particleIndex_t j) {
    std::swap(sortKeys[i], sortKeys[j]);
    std::swap(particleIndexToParticleIDMap[i], particleIndexToParticleIDMap[j]);
    std::swap(particleIDToParticleIndexMap[i], particleIDToParticleIndexMap[j]);

    std::swap(particleData[i], particleData[j]);

    // increase the index change counter since indices of particles have been changed
    this->indicesChangedCounter++;
}

FluidSolver::FluidParticle *FluidSolver::CompactParticleCollection::GetData() {
    return particleData.data();
}

void FluidSolver::CompactParticleCollection::Clear() {
    this->sortKeys.clear();
    this->particleIDToParticleIndexMap.clear();
    this->particleIndexToParticleIDMap.clear();
    this->particleData.clear();
}
