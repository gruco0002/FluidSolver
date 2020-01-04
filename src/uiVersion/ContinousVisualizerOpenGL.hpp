//
// Created by Corbinian Gruber on 19.12.19.
//

#ifndef FLUIDSOLVER_CONTINOUSVISUALIZEROPENGL_HPP
#define FLUIDSOLVER_CONTINOUSVISUALIZEROPENGL_HPP


#include <core/visualizer/ContinousVisualizer.hpp>
#include <core/basicScenarios/Scenario.hpp>
#include <engine/graphics/Texture2D.hpp>
#include <engine/RectangleRenderer.hpp>
#include "IOpenGLVisualizer.hpp"

class ContinousVisualizerOpenGL : public FluidSolver::ContinousVisualizer, public IOpenGLVisualizer {
public:
    Engine::Graphics::Texture2D *GetTexture() override;

public:
    void setRenderTargetSize(size_t width, size_t height) override;

    ContinousVisualizerOpenGL();

    void AfterRender(std::vector<Color> &data) override;

    virtual ~ContinousVisualizerOpenGL();

private:

    void RecreateTexture();

    Engine::Graphics::Texture2D *texture = nullptr;


};


#endif //FLUIDSOLVER_CONTINOUSVISUALIZEROPENGL_HPP
