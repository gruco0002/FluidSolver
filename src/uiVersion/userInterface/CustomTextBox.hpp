//
// Created by corbi on 03.12.2019.
//

#ifndef FLUIDSOLVER_CUSTOMTEXTBOX_HPP
#define FLUIDSOLVER_CUSTOMTEXTBOX_HPP

#include <CPPGUI.hpp>

namespace FluidUI {
    class CustomTextBox : public cppgui::Element {

    public:
        std::string Description = "";
        std::string InputText = "";
        std::function<void(std::string)> OnTextChanged = nullptr;

        CustomTextBox(const std::string &description, const std::string &inputText,
                      const std::function<void(std::string)> &onTextChanged);

    protected:
        Element *Build(cppgui::StateContext stateContext) override;

    };
}


#endif //FLUIDSOLVER_CUSTOMTEXTBOX_HPP
