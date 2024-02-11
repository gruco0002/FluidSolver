#include "UiElement.hpp"

namespace FluidStudio
{

    void UiElement::initialize(const UiData &ui_data)
    {
        this->ui_data = ui_data;
    }

    UiElement::~UiElement() = default;
} // namespace FluidStudio
