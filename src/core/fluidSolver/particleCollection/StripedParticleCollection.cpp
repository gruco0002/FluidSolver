
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
    particleIndexToDataIndexMap.push_back(0);
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
    while (this->particleIndexToDataIndexMap.size() <= index) {
        // add an entry to each array
        AddEntryToEachArray();
    }

    // set the index mapping data
    this->particleIndexToDataIndexMap[index] = index;

    return index;
}

void FluidSolver::StripedParticleCollection::SwapElements(size_t i, size_t j) {
    std::swap(particleIndexToDataIndexMap[i], particleIndexToDataIndexMap[j]);
    std::swap(sortKeys[i], sortKeys[j]);

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

void FluidSolver::StripedParticleCollection::InsertionSort(
        const FluidSolver::StripedParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // pre calculate the keys:
    PrecalculateSortKeys(sortKeyFunction);

    // start insertion sort
    for (size_t i = 1; i < size; i++) {
        auto key = sortKeys[i];
        size_t j = i - 1;
        while (j >= 0 && sortKeys[j] > key) {
            SwapElements(j, j + 1);
            j--;
        }
    }

}

void
FluidSolver::StripedParticleCollection::MergeSort(const FluidSolver::StripedParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // pre calculate the keys:
    PrecalculateSortKeys(sortKeyFunction);

    // define merge, left index is inclusive, right index is exclusive
    auto merge = [=](size_t left, size_t half, size_t right) {
        size_t leftIndex = left;
        size_t rightIndex = half;

        if (sortKeys[rightIndex - 1] <= sortKeys[rightIndex])
            return; // everything was already sorted

        while (leftIndex < half && rightIndex < right) {
            if (sortKeys[leftIndex] <= sortKeys[rightIndex]) {
                leftIndex++; // the element is already in the right place
            } else {
                // move the item by continous swapping to the target, then adapt half and other indices accordingly
                size_t movingIndex = rightIndex;
                while (movingIndex > leftIndex) {
                    SwapElements(movingIndex, movingIndex - 1);
                    movingIndex--;
                }

                rightIndex++;
                half++;
                leftIndex++;
            }
        }

    };


    // define merge sort, left index is inclusive, right index is exclusive
    std::function<void(size_t, size_t)> mergesort = [=](size_t left, size_t right) {
        if (right - left <= 1)
            return;

        size_t half = left + (right - left) / 2;
        mergesort(left, half);
        mergesort(half, right);

        merge(left, half, right);
    };

    // start mergesort
    mergesort(0, size);
}

void FluidSolver::StripedParticleCollection::PrecalculateSortKeys(
        const FluidSolver::StripedParticleCollection::sortKeyFunction_t &sortKeyFunction) {
    // TODO: parallelization
    for (size_t i = 0; i < size; i++) {
        sortKeys[i] = sortKeyFunction(i);
    }
}

void
FluidSolver::StripedParticleCollection::AddParticles(const std::vector<FluidParticle> &particles) {
    for (auto particle: particles)
        AddParticle(particle);
}

float FluidSolver::StripedParticleCollection::GetMass(uint32_t index) {
    return massData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetMass(uint32_t index, float value) {
    massData[particleIndexToDataIndexMap[index]] = value;
}

float FluidSolver::StripedParticleCollection::GetPressure(uint32_t index) {
    return pressureData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetPressure(uint32_t index, float value) {
    pressureData[particleIndexToDataIndexMap[index]] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetPosition(uint32_t index) {
    return positionData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetPosition(uint32_t index, glm::vec2 value) {
    positionData[particleIndexToDataIndexMap[index]] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetVelocity(uint32_t index) {
    return velocityData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetVelocity(uint32_t index, glm::vec2 value) {
    velocityData[particleIndexToDataIndexMap[index]] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetAcceleration(uint32_t index) {
    return accelerationData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetAcceleration(uint32_t index, glm::vec2 value) {
    accelerationData[particleIndexToDataIndexMap[index]] = value;
}

uint32_t FluidSolver::StripedParticleCollection::GetSize() {
    return size;
}

float FluidSolver::StripedParticleCollection::GetDensity(uint32_t index) {
    return densityData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetDensity(uint32_t index, float value) {
    densityData[particleIndexToDataIndexMap[index]] = value;
}

FluidSolver::ParticleType FluidSolver::StripedParticleCollection::GetParticleType(uint32_t index) {
    return (FluidSolver::ParticleType) typeData[particleIndexToDataIndexMap[index]];
}

void
FluidSolver::StripedParticleCollection::SetParticleType(uint32_t index, FluidSolver::ParticleType value) {
    typeData[particleIndexToDataIndexMap[index]] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetNonPressureAcceleration(uint32_t index) {
    return nonPressureAccelerationData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetNonPressureAcceleration(uint32_t index, glm::vec2 value) {
    nonPressureAccelerationData[particleIndexToDataIndexMap[index]] = value;
}

glm::vec2 FluidSolver::StripedParticleCollection::GetPredictedVelocity(uint32_t index) {
    return predictedVelocityData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetPredictedVelocity(uint32_t index, glm::vec2 value) {
    predictedVelocityData[particleIndexToDataIndexMap[index]] = value;
}

float FluidSolver::StripedParticleCollection::GetSourceTerm(uint32_t index) {
    return sourceTermData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetSourceTerm(uint32_t index, float value) {
    sourceTermData[particleIndexToDataIndexMap[index]] = value;
}

float FluidSolver::StripedParticleCollection::GetDiagonalElement(uint32_t index) {
    return diagonalElementData[particleIndexToDataIndexMap[index]];
}

void FluidSolver::StripedParticleCollection::SetDiagonalElement(uint32_t index, float value) {
    diagonalElementData[particleIndexToDataIndexMap[index]] = value;
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
