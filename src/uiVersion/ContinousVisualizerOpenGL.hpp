//
// Created by Corbinian Gruber on 19.12.19.
//

#ifndef FLUIDSOLVER_CONTINOUSVISUALIZEROPENGL_HPP
#define FLUIDSOLVER_CONTINOUSVISUALIZEROPENGL_HPP


#include <core/visualizer/ContinousVisualizer.hpp>
#include <core/basicScenarios/Scenario.hpp>
#include <engine/graphics/Texture2D.hpp>
#include <engine/RectangleRenderer.hpp>

class ContinousVisualizerOpenGL : public FluidSolver::ContinousVisualizer {
public:

    ContinousVisualizerOpenGL();

    void SetScenarioSize(FluidSolver::Scenario *scenario);

    void AfterRender(std::vector<Color> &data) override;

    virtual ~ContinousVisualizerOpenGL();

private:

    Engine::Graphics::Texture2D *texture = nullptr;

    Engine::RectangleRenderer *rectangleRenderer = nullptr;

};


#endif //FLUIDSOLVER_CONTINOUSVISUALIZEROPENGL_HPP
