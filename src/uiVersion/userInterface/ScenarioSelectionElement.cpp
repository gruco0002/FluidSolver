//
// Created by corbi on 02.12.2019.
//

#include "ScenarioSelectionElement.hpp"
#include "RootElement.hpp"

cppgui::Element *FluidUI::ScenarioSelectionElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto theme = stateContext.FindAncestor<Theme::ThemeData>();


    std::vector<Element *> scenarioButtonList;
    scenarioButtonList.push_back(BuildScenarioInfo(stateContext));
    scenarioButtonList.push_back(new Padding(new Text("Load Scenarios", theme->ControlFontSize), Padding::Border(0, 5)));
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
    using namespace cppgui;
    auto root = stateContext.FindAncestor<RootElement::RootState>();
    auto currentScenario = root->fluidSolverWindow->GetScenario();
    auto theme = stateContext.FindAncestor<Theme::ThemeData>();

    return new Column(std::vector<Element *>({

                                                     new Padding(new Text("Current", theme->ControlFontSize), Padding::Border(0,0,0, 5)),
                                                     new Row(std::vector<Element *>({

                                                                                            new Padding(new Column(
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
                                                                                                        Padding::Border(
                                                                                                                0, 5, 0,
                                                                                                                0)),
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
