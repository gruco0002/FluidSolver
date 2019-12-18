//
// Created by corbi on 18.12.2019.
//


#include "ContinousVisualizer.hpp"

void FluidSolver::ContinousVisualizer::setParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    ParticleCollection = particleCollection;
    if (neighborhoodSearch != nullptr) {
        delete neighborhoodSearch;
        neighborhoodSearch = new HashedNeighborhoodSearch(this->getParticleSize() * 3);
    }
}

FluidSolver::IParticleCollection *FluidSolver::ContinousVisualizer::getParticleCollection() {
    return ParticleCollection;
}

void FluidSolver::ContinousVisualizer::setParticleSize(float particleSize) {
    if (this->ParticleSize != particleSize || neighborhoodSearch == nullptr) {
        delete neighborhoodSearch;
        neighborhoodSearch = new HashedNeighborhoodSearch(particleSize * 3);
    }
    ParticleSize = particleSize;
    this->KernelSupport = 2.0f * particleSize;
}

float FluidSolver::ContinousVisualizer::getParticleSize() {
    return ParticleSize;
}

void FluidSolver::ContinousVisualizer::Render() {

    // find neighbors for all particles
    neighborhoodSearch->SetParticleCount(ParticleCollection->GetSize());
    neighborhoodSearch->SetupForPositionNeighborSearch(ParticleCollection);

    // clear data (set everything to clear color)
    std::fill(data.begin(), data.end(), ClearColor);

    // calculate color for each pixel
#pragma omp parallel for
    for (int64_t i = 0; i < Width * Height; i++) {
        size_t x = i % Width;
        size_t y = (i - (i % Width)) / Width;

        data[i] = CalculateColorForPixel(x, y);
    }

}

float FluidSolver::ContinousVisualizer::getRestDensity() {
    return RestDensity;
}

void FluidSolver::ContinousVisualizer::setRestDensity(float restDensity) {
    RestDensity = restDensity;
    MinimumRenderDensity = 0.8f * restDensity;
}

FluidSolver::ContinousVisualizer::ContinousVisualizer(size_t width, size_t height) : Width(width), Height(height) {
    data = std::vector<Color>(Width * Height);
}

FluidSolver::ContinousVisualizer::Color FluidSolver::ContinousVisualizer::CalculateColorForPixel(size_t x, size_t y) {

    glm::vec2 position = CalculatePositionForPixel(x, y);


    // calculate densities
    float boundaryDensity = 0.0f;
    float normalDensity = 0.0f;

    auto neighbors = neighborhoodSearch->GetParticleNeighborsByPosition(position, KernelSupport, ParticleCollection);

    for (uint32_t neighbor : neighbors) {
        auto type = ParticleCollection->GetParticleType(neighbor);
        if (type == IParticleCollection::ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }
        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighbor);
        float neighborMass = ParticleCollection->GetMass(neighbor);
        float densityContribution = neighborMass * kernel->GetKernelValue(neighborPosition, position, KernelSupport);

        if (type == IParticleCollection::ParticleTypeNormal) {
            normalDensity += densityContribution;
        } else if (type == IParticleCollection::ParticleTypeBoundary) {
            boundaryDensity += densityContribution;
        }

    }

    float density = normalDensity + boundaryDensity;

    // if density is too small return nothing
    if (density < MinimumRenderDensity)
        return Color(0, 0, 0);

    // check which type of density is larger and return the corresponding color
    if (normalDensity > boundaryDensity) {
        // fluid
        return Color(0, 0, 255);
    } else {
        // boundary
        return Color(128, 128, 128);
    }

}

glm::vec2 FluidSolver::ContinousVisualizer::CalculatePositionForPixel(size_t x, size_t y) {
    return glm::vec2();
}

FluidSolver::ContinousVisualizer::Color::Color(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {}

FluidSolver::ContinousVisualizer::Color::Color() {

}
