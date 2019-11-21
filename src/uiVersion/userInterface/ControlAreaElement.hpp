//
// Created by corbi on 21.11.2019.
//

#ifndef FLUIDSOLVER_CONTROLAREAELEMENT_HPP
#define FLUIDSOLVER_CONTROLAREAELEMENT_HPP

#include <CPPGUI.hpp>

namespace FluidUI {
    class ControlAreaElement : public cppgui::Element{

    protected:
        Element *Build(cppgui::StateContext stateContext) override;

    };
}


#endif //FLUIDSOLVER_CONTROLAREAELEMENT_HPP
