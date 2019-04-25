//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERTOPMENU_HPP
#define FLUIDSOLVER_FLUIDSOLVERTOPMENU_HPP


#include <dependencies/cppgui/src/Panel.hpp>
#include "ParticleRenderer.hpp"

class FluidSolverWindow;

class FluidSolverTopMenu : public cppgui::Panel{

public:
    FluidSolverTopMenu(ParticleRenderer* particleRenderer, FluidSolverWindow* window);

private:
    ParticleRenderer* particleRenderer;

    FluidSolverWindow* window;

    void Setup();

};


#endif //FLUIDSOLVER_FLUIDSOLVERTOPMENU_HPP
