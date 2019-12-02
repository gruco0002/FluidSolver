//
// Created by corbi on 02.12.2019.
//

#include "ScenarioSelectionElement.hpp"
#include "RootElement.hpp"

cppgui::Element *FluidUI::ScenarioSelectionElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    auto root = stateContext.FindAncestor<RootElement::RootState>();


    std::vector<Element *> scenarioButtonList;
    scenarioButtonList.push_back(BuildScenarioInfo(stateContext));
    for (FluidSolver::Scenario *ele: root->fluidSolverWindow->Scenarios) {
        scenarioButtonList.push_back(new Button(new Text(ele->GetName()), [=]() {
            root->fluidSolverWindow->SetScenario(ele);
        }));
    }

    return new Expander(new Text("Scenario"),
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

cppgui::Element *FluidUI::ScenarioSelectionElement::BuildScenarioInfo(cppgui::StateContext stateContext) {
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto currentScenario = root->fluidSolverWindow->GetScenario();

    using namespace cppgui;

    return new Column(std::vector<Element *>({

                                                     new Text("Current"),
                                                     new Row(std::vector<Element *>({

                                                                                            new Column(
                                                                                                    std::vector<Element *>(
                                                                                                            {
                                                                                                                    new Text(
                                                                                                                            "Name:"),
                                                                                                                    new Text(
                                                                                                                            "Particle Size:")
                                                                                                            }),
                                                                                                    MainAxisAlignmentStart,
                                                                                                    CrossAxisAlignmentStart,
                                                                                                    AxisSizeMin),
                                                                                            new Column(
                                                                                                    std::vector<Element *>(
                                                                                                            {
                                                                                                                    new Text(
                                                                                                                            currentScenario->GetName()),
                                                                                                                    new Text(
                                                                                                                            std::to_string(
                                                                                                                                    currentScenario->GetParticleSize()))
                                                                                                            }),
                                                                                                    MainAxisAlignmentStart,
                                                                                                    CrossAxisAlignmentStart,
                                                                                                    AxisSizeMin),

                                                                                    }))

                                             }), MainAxisAlignmentStart, CrossAxisAlignmentStart, AxisSizeMin);


}
