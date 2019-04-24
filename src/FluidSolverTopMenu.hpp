//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERTOPMENU_HPP
#define FLUIDSOLVER_FLUIDSOLVERTOPMENU_HPP


#include <dependencies/cppgui/src/Panel.hpp>
#include "ParticleRenderer.hpp"

class FluidSolverTopMenu : public cppgui::Panel{

public:
    FluidSolverTopMenu(ParticleRenderer* particleRenderer);

private:
    ParticleRenderer* particleRenderer;

    void Setup();

};


#endif //FLUIDSOLVER_FLUIDSOLVERTOPMENU_HPP
