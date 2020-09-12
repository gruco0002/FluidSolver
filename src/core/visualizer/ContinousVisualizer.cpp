#include "ContinousVisualizer.hpp"

void FluidSolver::ContinousVisualizer::setParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    ParticleCollection = particleCollection;
    if (neighborhoodSearch != nullptr) {
        delete neighborhoodSearch;
        neighborhoodSearch = nullptr;
        if (ParticleCollection != nullptr)
            neighborhoodSearch = new HashedNeighborhoodSearch(ParticleCollection, KernelSupport);
    }
}

FluidSolver::IParticleCollection *FluidSolver::ContinousVisualizer::getParticleCollection() {
    return ParticleCollection;
}

void FluidSolver::ContinousVisualizer::setParticleSize(float particleSize) {
    if (this->ParticleSize != particleSize || neighborhoodSearch == nullptr) {
        delete neighborhoodSearch;
        neighborhoodSearch = nullptr;
        if (ParticleCollection != nullptr)
            neighborhoodSearch = new HashedNeighborhoodSearch(ParticleCollection, KernelSupport);
    }
    ParticleSize = particleSize;

}

float FluidSolver::ContinousVisualizer::getParticleSize() {
    return ParticleSize;
}

void FluidSolver::ContinousVisualizer::Render() {


    // find neighbors for all particles
    neighborhoodSearch->UpdateGrid();

    // clear data (set everything to clear color) --> clearing is not needed, since we set each pixel value in the loop
    //std::fill(data.begin(), data.end(), ClearColor);

    // calculate color for each pixel
#pragma omp parallel for
    for (pIndex_t i = 0; i < Width * Height; i++) {
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

    // particle selection
    float maxNormalDensityContribution = 0.0f;
    bool maxNormalDensityContributerIsSelected = false;
    float maxBoundaryDensityContribution = 0.0f;
    bool maxBoundaryDensityContributerIsSelected = false;

    auto neighbors = neighborhoodSearch->GetNeighbors(position);

    for (uint32_t neighbor : neighbors) {
        auto type = ParticleCollection->GetParticleType(neighbor);
        if (type == ParticleTypeDead) {
            continue; // don*t calculate unnecessary values for dead particles.
        }
        glm::vec2 neighborPosition = ParticleCollection->GetPosition(neighbor);
        float neighborMass = ParticleCollection->GetMass(neighbor);
        float densityContribution = neighborMass * kernel->GetKernelValue(neighborPosition, position);

        if (type == ParticleTypeNormal) {
            normalDensity += densityContribution;
            if (maxNormalDensityContribution <= densityContribution) {
                maxNormalDensityContribution = densityContribution;
                maxNormalDensityContributerIsSelected = particleSelection->IsParticleSelected(neighbor,
                                                                                              ParticleCollection);
            }
        } else if (type == ParticleTypeBoundary) {
            boundaryDensity += densityContribution;
            if (maxBoundaryDensityContribution <= densityContribution) {
                maxBoundaryDensityContribution = densityContribution;
                maxBoundaryDensityContributerIsSelected = particleSelection->IsParticleSelected(neighbor,
                                                                                                ParticleCollection);
            }
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
        if (this->VisualizeParticleSelection && maxBoundaryDensityContributerIsSelected)
            return Color(128, 0, 0);
        return Color(128, 128, 128);
    } else {
        // fluid
        if (this->VisualizeParticleSelection && maxNormalDensityContributerIsSelected)
            return Color(255, 0, 0);
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
    return CalculatePositionForPixel(pixelX, pixelY);
}

void FluidSolver::ContinousVisualizer::setParticleSelection(FluidSolver::IParticleSelection *particleSelection) {
    this->particleSelection = particleSelection;
}

FluidSolver::IParticleSelection *FluidSolver::ContinousVisualizer::getParticleSelection() {
    return particleSelection;
}

FluidSolver::IKernel *FluidSolver::ContinousVisualizer::getKernel() const {
    return kernel;
}

void FluidSolver::ContinousVisualizer::setKernel(FluidSolver::IKernel *kernel) {
    ContinousVisualizer::kernel = kernel;
}

FluidSolver::ContinousVisualizer::Color::Color(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {}

FluidSolver::ContinousVisualizer::Color::Color() {

}

FluidSolver::ContinousVisualizer::Color::Color(glm::vec3 color) {
    R = (unsigned char) (color.r * 255.0f);
    G = (unsigned char) (color.g * 255.0f);
    B = (unsigned char) (color.b * 255.0f);
}
