//
// Created by corbi on 21.11.2019.
//

#include "ControlAreaElement.hpp"
#include "SimulationControlElement.hpp"

cppgui::Element *FluidUI::ControlAreaElement::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;

    Element* top =  new Row(std::vector<Element*>(
                                                           {
                                                               new Padding(new Text("Fluid Solver", 25.0f),
                                                                        Padding::Border(5.0f, 10.0f)),

                                                            new Padding(
                                                                    new Button(new Icon(new GuiIconSource(GetState()->Expanded?"keyboard_arrow_right" : "keyboard_arrow_left")), [=]() {
                                                                        GetState()->Expanded = !GetState()->Expanded;
                                                                        OnStateChanged();
                                                                    }), Padding::Border(5.0f, 10.0f)),
                                                           }), MainAxisAlignmentSpaceBetween)

                                           ;
    if (GetState()->Expanded) {

        top = new Column(std::vector<Element*>({top,new Expanded(new ScrollView(new Column(std::vector<Element *>({
                                                                                                                          new SimulationControlElement(),


                                                                                                                  }),
                                                                                           MainAxisAlignmentStart,
                                                                                           CrossAxisAlignmentStart,
                                                                                           AxisSizeMin))) }));
    }

    return new Container(top, Colors::White(), 300.0f, NullValue,GetState()->Expanded);

}

cppgui::ElementState *FluidUI::ControlAreaElement::CreateState() {
    return new ControlAreaElementState();
}

bool FluidUI::ControlAreaElement::StateMatchElement(cppgui::ElementState *state) {
    return dynamic_cast<ControlAreaElementState *>(state) != nullptr;
}

FluidUI::ControlAreaElement::ControlAreaElementState *FluidUI::ControlAreaElement::GetState() {
    return dynamic_cast<ControlAreaElementState *>(state);
}
