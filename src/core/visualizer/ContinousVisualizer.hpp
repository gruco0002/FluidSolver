//
// Created by corbi on 18.12.2019.
//

#ifndef FLUIDSOLVER_CONTINOUSVISUALIZER_HPP
#define FLUIDSOLVER_CONTINOUSVISUALIZER_HPP

#include <core/visualizer/ISimulationVisualizer.hpp>
#include <vector>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/kernel/IKernel.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>

namespace FluidSolver {
    class ContinousVisualizer : public ISimulationVisualizer {

    public:
        void setSimulationViewArea(FluidSolver::ISimulationVisualizer::SimulationViewArea viewArea) override;

        void setRenderTargetSize(size_t width, size_t height) override;

        struct Color {
            unsigned char R = 0;
            unsigned char G = 0;
            unsigned char B = 0;

            Color(unsigned char r, unsigned char g, unsigned char b);

            Color();

            Color(glm::vec3 color);

        };


        ContinousVisualizer(size_t width, size_t height);

        void setParticleCollection(IParticleCollection *particleCollection) override;

        IParticleCollection *getParticleCollection() override;

        void setParticleSize(float particleSize) override;

        float getParticleSize() override;

        void Render() override;

        float getRestDensity() override;

        void setRestDensity(float restDensity) override;

        virtual void AfterRender(std::vector<Color> &data);

        size_t getWidth() const;

        void setWidth(size_t width);

        size_t getHeight() const;

        void setHeight(size_t height);

        glm::vec3 ClearColor = glm::vec3(1.0f, 1.0f, 1.0f);
        float MinimumRenderDensity = 0.0f;

        glm::vec2 ConvertPixelCoordinateToParticleSpace(size_t pixelX, size_t pixelY) override;

    private:

        SimulationViewArea viewArea;

        struct Viewport {
            float Top = 0.0f;
            float Left = 0.0f;

            float Width = 0.0f;
            float Height = 0.0f;
        };

        Viewport viewport;

        void recalculateViewportData();

        Viewport FitViewportToAspectRation(Viewport value);

        float ParticleSize = 0.0f;
        IParticleCollection *ParticleCollection = nullptr;
        float RestDensity = 0.0f;


        size_t Width = 1920;
        size_t Height = 1080;


        std::vector<Color> data;

        HashedNeighborhoodSearch *neighborhoodSearch = nullptr;
        IKernel *kernel = new CubicSplineKernel();
        float KernelSupport = 0.0f;

        Color CalculateColorForPixel(size_t x, size_t y);

        glm::vec2 CalculatePositionForPixel(size_t x, size_t y);

    };
}


#endif //FLUIDSOLVER_CONTINOUSVISUALIZER_HPP
