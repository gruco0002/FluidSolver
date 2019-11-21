//
// Created by corbi on 21.11.2019.
//

#ifndef FLUIDSOLVER_SIMULATIONCONTROLELEMENT_HPP
#define FLUIDSOLVER_SIMULATIONCONTROLELEMENT_HPP

#include <CPPGUI.hpp>

namespace FluidUI {
    class SimulationControlElement : public cppgui::Element {

    protected:
        Element *Build(cppgui::StateContext stateContext) override;

    };
}


#endif //FLUIDSOLVER_SIMULATIONCONTROLELEMENT_HPP
