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

    // clear data (set everything to clear color) --> clearing is not needed, since we set each pixel value in the loop
    //std::fill(data.begin(), data.end(), ClearColor);

    // calculate color for each pixel
#pragma omp parallel for
    for (int64_t i = 0; i < Width * Height; i++) {
        size_t x = i % Width;
        size_t y = (i - (i % Width)) / Width;

        data[i] = CalculateColorForPixel(x, y);
    }


    AfterRender(data);

}

float FluidSolver::ContinousVisualizer::getRestDensity() {
    return RestDensity;
}

void FluidSolver::ContinousVisualizer::setRestDensity(float restDensity) {
    RestDensity = restDensity;
    MinimumRenderDensity = 0.4f * restDensity;
}

FluidSolver::ContinousVisualizer::ContinousVisualizer(size_t width, size_t height) : Width(width), Height(height) {
    data = std::vector<Color>(Width * Height);
    recalculateViewportData();
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
        return Color(ClearColor);

    // check which type of density is larger and return the corresponding color
    // first of, boundary density should dominate on a boundary
    if (boundaryDensity >= MinimumRenderDensity) {
        // boundary
        return Color(128, 128, 128);
    } else {
        // fluid
        return Color(0, 0, 255);
    }

}

glm::vec2 FluidSolver::ContinousVisualizer::CalculatePositionForPixel(size_t x, size_t y) {
    float xCoord = viewport.Left;
    float yCoord = viewport.Top;
    xCoord += x / (float) Width * viewport.Width;
    yCoord -= y / (float) Height * viewport.Height;
    return glm::vec2(xCoord, yCoord);
}

void FluidSolver::ContinousVisualizer::AfterRender(std::vector<Color> &data) {
    // do nothing, this is meant for other classes
}

FluidSolver::ContinousVisualizer::Viewport
FluidSolver::ContinousVisualizer::FitViewportToAspectRation(FluidSolver::ContinousVisualizer::Viewport value) {
    Viewport res;
    res.Top = value.Top;
    res.Left = value.Left;

    if (value.Width / value.Height * (float) Height < (float) Width) {
        // height is okay, width must be larger
        res.Height = value.Height;
        res.Width = (float) Width / (float) Height * value.Height;

        // center the additional gained width
        res.Left -= (res.Width - value.Width) / 2.0f;
    } else {
        // width is okay, height must be larger
        res.Width = value.Width;
        res.Height = (float) Height / (float) Width * value.Width;

        // center the additional gained height
        res.Top += (res.Height - value.Height) / 2.0f;
    }

    return res;
}

size_t FluidSolver::ContinousVisualizer::getWidth() const {
    return Width;
}

void FluidSolver::ContinousVisualizer::setWidth(size_t width) {
    Width = width;
    data.resize(Width * Height);
    recalculateViewportData();
}

size_t FluidSolver::ContinousVisualizer::getHeight() const {
    return Height;
}

void FluidSolver::ContinousVisualizer::setHeight(size_t height) {
    Height = height;
    data.resize(Width * Height);
    recalculateViewportData();
}

void FluidSolver::ContinousVisualizer::setSimulationViewArea(
        FluidSolver::ISimulationVisualizer::SimulationViewArea viewArea) {
    this->viewArea = viewArea;
    recalculateViewportData();
}

void FluidSolver::ContinousVisualizer::setRenderTargetSize(size_t width, size_t height) {
    Width = width;
    Height = height;
    data.resize(Width * Height);
    recalculateViewportData();
}

void FluidSolver::ContinousVisualizer::recalculateViewportData() {
    Viewport port;
    port.Top = viewArea.Top;
    port.Left = viewArea.Left;
    port.Width = viewArea.Right - viewArea.Left;
    port.Height = viewArea.Top - viewArea.Bottom;

    this->viewport = this->FitViewportToAspectRation(port);
}

glm::vec2 FluidSolver::ContinousVisualizer::ConvertPixelCoordinateToParticleSpace(size_t pixelX, size_t pixelY) {
    float xCoord = (float) pixelX;
    float yCoord = (float) pixelY;
    xCoord -= viewport.Left;
    yCoord -= viewport.Top;
    yCoord *= -1.0f;

    xCoord = xCoord * (float) Width / viewport.Width;
    yCoord = yCoord * (float) Height / viewport.Height;

    return glm::vec2(xCoord, yCoord);
}

FluidSolver::ContinousVisualizer::Color::Color(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {}

FluidSolver::ContinousVisualizer::Color::Color() {

}

FluidSolver::ContinousVisualizer::Color::Color(glm::vec3 color) {
    R = (unsigned char) (color.r * 255.0f);
    G = (unsigned char) (color.g * 255.0f);
    B = (unsigned char) (color.b * 255.0f);
}
