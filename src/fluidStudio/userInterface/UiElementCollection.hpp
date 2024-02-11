#pragma once

#include <map>
#include <typeindex>

#include "LibFluidAssert.hpp"

namespace FluidStudio
{
    class UiElement;

    class UiElementCollection
    {
      private:
        std::map<std::type_index, UiElement *> ui_elements;

      public:
        UiElementCollection() = default;

        template <typename T> void add(T &ui_element)
        {
            ui_elements[typeid(T *)] = &ui_element;
        }

        template <typename T> T &get()
        {
            FLUID_ASSERT(ui_elements.find(typeid(T *)) != ui_elements.end(),
                         "Could not find ui element of specified type!");

            return *dynamic_cast<T *>(ui_elements[typeid(T *)]);
        }
    };
} // namespace FluidStudio
