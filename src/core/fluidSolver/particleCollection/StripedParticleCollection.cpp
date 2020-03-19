
#include "StripedParticleCollection.hpp"

size_t FluidSolver::StripedParticleCollection::AddParticle(
        const FluidSolver::FluidParticle &fluidParticle) {

    // Create and add an empty particle
    auto index = AddEmptyParticle();

    // set the particle data
    typeData[index] = fluidParticle.Type;

    positionData[index] = fluidParticle.Position;
    velocityData[index] = fluidParticle.Velocity;
    accelerationData[index] = fluidParticle.Acceleration;
    massData[index] = fluidParticle.Mass;
    pressureData[index] = fluidParticle.Pressure;
    densityData[index] = fluidParticle.Density;
    nonPressureAccelerationData[index] = fluidParticle.NonPressureAcceleration;
    predictedVelocityData[index] = fluidParticle.PredictedVelocity;
    sourceTermData[index] = fluidParticle.SourceTerm;
    diagonalElementData[index] = fluidParticle.DiagonalElement;

    return index;

}

void FluidSolver::StripedParticleCollection::AddEntryToEachArray() {
    particleIDToParticleIndexMap.push_back(0);
    particleIndexToParticleIDMap.push_back(0);
    sortKeys.push_back(0);

    typeData.push_back(0);
    positionData.push_back(glm::vec2(0.0f));
    velocityData.push_back(glm::vec2(0.0f));
    accelerationData.push_back(glm::vec2(0.0f));
    massData.push_back(0.0f);
    pressureData.push_back(0.0f);
    densityData.push_back(0.0f);
    nonPressureAccelerationData.push_back(glm::vec2(0.0f));
    predictedVelocityData.push_back(glm::vec2(0.0f));
    sourceTermData.push_back(0.0f);
    diagonalElementData.push_back(0.0f);
}

size_t FluidSolver::StripedParticleCollection::AddEmptyParticle() {
    // get an index for the particle
    auto index = this->size;
    this->size++;

    // check if there is enough space in the data arrays, if not create them
    while (this->particleIDToParticleIndexMap.size() <= index) {
        // add an entry to each array
        AddEntryToEachArray();
    }

    // set the index mapping data
    this->particleIDToParticleIndexMap[index] = index;
    this->particleIndexToParticleIDMap[index] = index;

    return index;
}

void FluidSolver::StripedParticleCollection::SwapElements(size_t i, size_t j) {
    std::swap(particleIDToParticleIndexMap[i], particleIDToParticleIndexMap[j]);
    std::swap(sortKeys[i], sortKeys[j]);
    std::swap(particleIndexToParticleIDMap[i], particleIndexToParticleIDMap[j]);

    std::swap(typeData[i], typeData[j]);
    std::swap(positionData[i], positionData[j]);
    std::swap(velocityData[i], velocityData[j]);
    std::swap(accelerationData[i], accelerationData[j]);
    std::swap(massData[i], massData[j]);
    std::swap(pressureData[i], pressureData[j]);
    std::swap(densityData[i], densityData[j]);
    std::swap(nonPressureAccelerationData[i], nonPressureAccelerationData[j]);
    std::swap(predictedVelocityData[i], predictedVelocityData[j]);
    std::swap(sourceTermData[i], sourceTermData[j]);
    std::swap(diagonalElementData[i], diagonalElementData[j]);
}


void FluidSolver::StripedParticleCollection::PrecalculateSortKeys(
        const FluidSolver::StripedParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // TODO: parallelization
    for (size_t i = 0; i < size; i++) {
        sortKeys[i] = sortKeyFunction(i);
    }
}


float FluidSolver::StripedParticleCollection::GetMass(uint32_t index) {
    return massData[index];
}

void FluidSolver::StripedParticleCollection::SetMass(uint32_t index, float value) {
    massData[index] = value;
}

float FluidSolver::StripedParticleCollection::GetPressure(uint32_t index) {
    return pressureData[index];
}

void FluidSolver::StripedParticleCollection::SetPressure(uint32_t index, float value) {
    pressureData[index] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetPosition(uint32_t index) {
    return positionData[index];
}

void FluidSolver::StripedParticleCollection::SetPosition(uint32_t index, glm::vec2 value) {
    positionData[index] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetVelocity(uint32_t index) {
    return velocityData[index];
}

void FluidSolver::StripedParticleCollection::SetVelocity(uint32_t index, glm::vec2 value) {
    velocityData[index] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetAcceleration(uint32_t index) {
    return accelerationData[index];
}

void FluidSolver::StripedParticleCollection::SetAcceleration(uint32_t index, glm::vec2 value) {
    accelerationData[index] = value;
}

uint32_t FluidSolver::StripedParticleCollection::GetSize() {
    return size;
}

float FluidSolver::StripedParticleCollection::GetDensity(uint32_t index) {
    return densityData[index];
}

void FluidSolver::StripedParticleCollection::SetDensity(uint32_t index, float value) {
    densityData[index] = value;
}

FluidSolver::ParticleType FluidSolver::StripedParticleCollection::GetParticleType(uint32_t index) {
    return (FluidSolver::ParticleType) typeData[index];
}

void
FluidSolver::StripedParticleCollection::SetParticleType(uint32_t index, FluidSolver::ParticleType value) {
    typeData[index] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetNonPressureAcceleration(uint32_t index) {
    return nonPressureAccelerationData[index];
}

void FluidSolver::StripedParticleCollection::SetNonPressureAcceleration(uint32_t index, glm::vec2 value) {
    nonPressureAccelerationData[index] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetPredictedVelocity(uint32_t index) {
    return predictedVelocityData[index];
}

void FluidSolver::StripedParticleCollection::SetPredictedVelocity(uint32_t index, glm::vec2 value) {
    predictedVelocityData[index] = value;
}

float FluidSolver::StripedParticleCollection::GetSourceTerm(uint32_t index) {
    return sourceTermData[index];
}

void FluidSolver::StripedParticleCollection::SetSourceTerm(uint32_t index, float value) {
    sourceTermData[index] = value;
}

float FluidSolver::StripedParticleCollection::GetDiagonalElement(uint32_t index) {
    return diagonalElementData[index];
}

void FluidSolver::StripedParticleCollection::SetDiagonalElement(uint32_t index, float value) {
    diagonalElementData[index] = value;
}

uint8_t *FluidSolver::StripedParticleCollection::TypeData() {
    return typeData.data();
}

glm::vec2 *FluidSolver::StripedParticleCollection::PositionData() {
    return positionData.data();
}

glm::vec2 *FluidSolver::StripedParticleCollection::VelocityData() {
    return velocityData.data();
}

glm::vec2 *FluidSolver::StripedParticleCollection::AccelerationData() {
    return accelerationData.data();
}

float *FluidSolver::StripedParticleCollection::MassData() {
    return massData.data();
}

float *FluidSolver::StripedParticleCollection::PressureData() {
    return pressureData.data();
}

float *FluidSolver::StripedParticleCollection::DensityData() {
    return densityData.data();
}

uint64_t FluidSolver::StripedParticleCollection::GetSortKey(size_t index) {
    return sortKeys[index];
}

size_t FluidSolver::StripedParticleCollection::GetParticleID(size_t index) {
    return particleIndexToParticleIDMap[index];
}

size_t FluidSolver::StripedParticleCollection::GetIndex(size_t particleID) {
    return particleIDToParticleIndexMap[particleID];
}

void FluidSolver::StripedParticleCollection::Clear() {
    this->size = 0;

    particleIDToParticleIndexMap.clear();
    particleIndexToParticleIDMap.clear();
    sortKeys.clear();

    typeData.clear();
    positionData.clear();
    velocityData.clear();
    accelerationData.clear();
    massData.clear();
    pressureData.clear();
    densityData.clear();
    nonPressureAccelerationData.clear();
    predictedVelocityData.clear();
    sourceTermData.clear();
    diagonalElementData.clear();
}
