//
// Created by corbi on 02.12.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERELEMENT_HPP
#define FLUIDSOLVER_FLUIDSOLVERELEMENT_HPP

#include <CPPGUI.hpp>
#include "core/fluidSolver/IFluidSolver.hpp"

namespace FluidUI {
    class FluidSolverElement : public cppgui::Element {

    private:
        class FluidSolverElementState : public cppgui::ElementState {
        public:
            enum FluidSolverType {
                FluidSolverTypeIISPH,
                FluidSolverTypeSimpleSPH,
            };

            bool IsExpanded = true;
            FluidSolverType Type = FluidSolverTypeIISPH;

            // IISPH Stuff
            std::string minIterationsString = "";
            std::string maxDensityErrorAllowedString = "";
            std::string omegaString = "";
            std::string gammaString = "";

            // Simple SPH
            std::string stiffnessString = "";
            std::string viscosityString = "";

            void InitState(cppgui::StateContext stateContext) override;

            void FluidSolverChanged(FluidSolver::IFluidSolver *fluidSolver);
        };

        FluidSolverElementState *GetState();

    protected:
        Element *Build(cppgui::StateContext stateContext) override;

        cppgui::ElementState *CreateState() override;

        bool StateMatchElement(cppgui::ElementState *state) override;


    };
}


#endif //FLUIDSOLVER_FLUIDSOLVERELEMENT_HPP
