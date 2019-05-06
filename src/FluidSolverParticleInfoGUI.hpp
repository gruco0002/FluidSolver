//
// Created by Corbinian Gruber on 2019-05-06.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERPARTICLEINFOGUI_HPP
#define FLUIDSOLVER_FLUIDSOLVERPARTICLEINFOGUI_HPP

#include <cppgui/src/Panel.hpp>
#include <cppgui/src/StyledLabel.hpp>
#include "core/interface/IParticleCollection.hpp"


class FluidSolverParticleInfoGUI : public cppgui::Panel {

public:
    FluidSolverParticleInfoGUI(uint32_t particleIndex, FluidSolver::IParticleCollection *particleCollection);

    FluidSolver::IParticleCollection *particleCollection;

    uint32_t particleIndex;

    void UpdateData();

private:

    cppgui::StyledLabel *id;
    cppgui::StyledLabel *position;
    cppgui::StyledLabel *velocity;

    void Setup();

};


#endif //FLUIDSOLVER_FLUIDSOLVERPARTICLEINFOGUI_HPP
