#include "Kernel.hpp"
#include "uiVersion/FluidSolverWindow.hpp"
#include "imguiHelper.hpp"
#include "core/fluidSolver/kernel/CubicSplineKernel.hpp"
#include "core/fluidSolver/kernel/PreCalculatedCubicSplineKernel.hpp"

FluidUI::Kernel::Kernel(FluidSolverWindow *window) {
    this->window = window;
}

void FluidUI::Kernel::Run(bool *viewKernel) {
    if (window == nullptr)
        return;
    Init();
    CheckChange();

    if (!(*viewKernel))return;

    if (ImGui::Begin("Kernel Settings", viewKernel)) {

        ImGui::Combo("Type", (int *) &currentType, names, IM_ARRAYSIZE(names));

        if (currentType == CubicSpline) {
            auto cubic = dynamic_cast<FluidSolver::CubicSplineKernel *>(window->GetSimulation()->getKernel());
        } else if (currentType == PreCalcCubicSpline) {
            auto pre = dynamic_cast<FluidSolver::PreCalculatedCubicSplineKernel *>(window->GetSimulation()->getKernel());
        }

    }
    ImGui::End();
}

const char *FluidUI::Kernel::names[] = {"Cubic Spline", "Precalculated Cubic Spline"};

void FluidUI::Kernel::Init() {
    if (isInit)return;
    isInit = true;

    auto kernel = window->GetSimulation()->getKernel();
    if (dynamic_cast<FluidSolver::CubicSplineKernel *>(kernel) != nullptr) {
        currentType = CubicSpline;
    } else if (dynamic_cast<FluidSolver::PreCalculatedCubicSplineKernel *>(kernel) != nullptr) {
        currentType = PreCalcCubicSpline;
    }

}

void FluidUI::Kernel::CheckChange() {
    auto kernel = window->GetSimulation()->getKernel();

    if (currentType == CubicSpline) {
        if (dynamic_cast<FluidSolver::CubicSplineKernel *>(kernel) == nullptr) {
            window->GetSimulation()->setKernel(
                    new FluidSolver::CubicSplineKernel(window->GetSimulation()->getRadius()));
            delete kernel;
        }
    } else if (currentType == PreCalcCubicSpline) {
        if (dynamic_cast<FluidSolver::PreCalculatedCubicSplineKernel *>(kernel) == nullptr) {
            window->GetSimulation()->setKernel(
                    new FluidSolver::PreCalculatedCubicSplineKernel(window->GetSimulation()->getRadius()));
            delete kernel;
        }
    }
}
