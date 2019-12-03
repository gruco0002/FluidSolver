//
// Created by corbi on 03.12.2019.
//

#include "CustomTextBox.hpp"

cppgui::Element *FluidUI::CustomTextBox::Build(cppgui::StateContext stateContext) {
    using namespace cppgui;
    return new Column(std::vector<Element *>({
                                                     new Text(Description),
                                                     new TextBox(InputText, OnTextChanged)
                                             }), MainAxisAlignmentStart, CrossAxisAlignmentStart, AxisSizeMin);
}

FluidUI::CustomTextBox::CustomTextBox(const std::string &description, const std::string &inputText,
                                      const std::function<void(std::string)> &onTextChanged) : Description(description),
                                                                                               InputText(inputText),
                                                                                               OnTextChanged(
                                                                                                       onTextChanged) {}
