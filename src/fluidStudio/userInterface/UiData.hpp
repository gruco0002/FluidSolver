#pragma once

#include "FluidSolverWindow.hpp"
#include "userInterface/UiElementCollection.hpp"


namespace FluidStudio {

    struct UiData {
      public:
        FluidSolverWindow& window() const;

        UiElementCollection& collection() const;

        explicit UiData(FluidSolverWindow* window, UiElementCollection& collection);
        UiData();

      private:
        FluidSolverWindow* m_window = nullptr;
        UiElementCollection* m_collection = nullptr;
    };


} // namespace FluidStudio