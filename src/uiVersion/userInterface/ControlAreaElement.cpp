//
// Created by corbi on 21.11.2019.
//

#include "ControlAreaElement.hpp"

cppgui::Element *FluidUI::ControlAreaElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;
    return new Column(std::vector<Element *>({
                                                     new Padding(new Text("Fluid Solver", 25.0f),
                                                                 Padding::Border(5.0f, 10.0f)),
                                                     new Expanded(new ScrollView(nullptr))
                                             }));
}
