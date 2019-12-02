//
// Created by corbi on 02.12.2019.
//

#include "ScenarioSelectionElement.hpp"
#include "RootElement.hpp"

cppgui::Element *FluidUI::ScenarioSelectionElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto currentScenario = root->fluidSolverWindow->GetScenario();

    std::vector<Element *> scenarioButtonList;
    for (FluidSolver::Scenario *ele: root->fluidSolverWindow->Scenarios) {
        scenarioButtonList.push_back(new Button(new Text(ele->GetName()), [=]() {
            root->fluidSolverWindow->SetScenario(ele);
        }));
    }

    return new Expander(new Text("Load Scenarios"),
                        new Column(scenarioButtonList, MainAxisAlignmentStart, CrossAxisAlignmentStart,
                        AxisSizeMin), GetState()->IsExpanded, [=](bool isExpanded) {
        GetState()->IsExpanded = isExpanded;
        OnStateChanged();
    });
}

cppgui::ElementState *FluidUI::ScenarioSelectionElement::CreateState() {
    return new ScenarioSelectionElementState();
}

bool FluidUI::ScenarioSelectionElement::StateMatchElement(cppgui::ElementState *state) {
    return dynamic_cast<ScenarioSelectionElementState *>(state) != nullptr;
}

FluidUI::ScenarioSelectionElement::ScenarioSelectionElementState *FluidUI::ScenarioSelectionElement::GetState() {
    return dynamic_cast<ScenarioSelectionElementState *>(state);
}
