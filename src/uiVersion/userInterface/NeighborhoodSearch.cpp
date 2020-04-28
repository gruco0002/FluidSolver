#include "NeighborhoodSearch.hpp"
#include <uiVersion/FluidSolverWindow.hpp>
#include <imguiHelper.hpp>
#include <core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp>

FluidUI::NeighborhoodSearch::NeighborhoodSearch(FluidSolverWindow *window) : window(window) {}

void FluidUI::NeighborhoodSearch::Run(bool *viewNeighborhoodSearch) {
    Init();
    CheckChange();

    if(window == nullptr)
        return;

    if (!(*viewNeighborhoodSearch))return;

    if (ImGui::Begin("Neighborhood Search", viewNeighborhoodSearch)) {

        // selection
        ImGui::Combo("Type", (int *) &currentType, names, IM_ARRAYSIZE(names));

        if (currentType == SearchCompactHashing) {
            auto comp = dynamic_cast<FluidSolver::CompactHashingNeighborhoodSearch*>(window->GetSimulation()->getNeighborhoodSearch());

        } else if (currentType == SearchHashed) {
            auto hashed = dynamic_cast<FluidSolver::HashedNeighborhoodSearch*>(window->GetSimulation()->getNeighborhoodSearch());

        }


    }
    ImGui::End();

}

const char *FluidUI::NeighborhoodSearch::names[] = {"Compact Hashing", "Hashed"};

void FluidUI::NeighborhoodSearch::Init() {
    if (window == nullptr)return;
    if (isInit)return;
    isInit = true;

    auto sim = window->GetSimulation();
    auto search = sim->getNeighborhoodSearch();
    if (dynamic_cast<FluidSolver::CompactHashingNeighborhoodSearch *>(search)) {
        currentType = SearchCompactHashing;
    } else if (dynamic_cast<FluidSolver::HashedNeighborhoodSearch *>(search)) {
        currentType = SearchHashed;
    }

}

void FluidUI::NeighborhoodSearch::CheckChange() {
    if (window == nullptr)return;
    auto sim = window->GetSimulation();
    auto search = sim->getNeighborhoodSearch();
    if (currentType == SearchCompactHashing) {
        if (dynamic_cast<FluidSolver::CompactHashingNeighborhoodSearch *>(search) == nullptr) {
            sim->setNeighborhoodSearch(
                    new FluidSolver::CompactHashingNeighborhoodSearch(sim->getParticleCollection(), sim->getRadius()));
            delete search;
        }
    } else if (currentType == SearchHashed) {
        if (dynamic_cast<FluidSolver::HashedNeighborhoodSearch *>(search) == nullptr) {
            sim->setNeighborhoodSearch(
                    new FluidSolver::HashedNeighborhoodSearch(sim->getParticleCollection(), sim->getRadius()));
            delete search;
        }
    }
}

