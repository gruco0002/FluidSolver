#pragma once

#include "FluidSolverWindow.hpp"
#include "userInterface/UiElementCollection.hpp"


namespace FluidStudio {

    class UiLayer;

    struct UiData {
      public:
        FluidSolverWindow& window() const;

        UiElementCollection& collection() const;

        UiLayer& uiLayer() const;

        explicit UiData(FluidSolverWindow* window, UiElementCollection& collection, UiLayer* uiLayer);
        UiData();

      private:
        FluidSolverWindow* m_window = nullptr;
        UiElementCollection* m_collection = nullptr;
        UiLayer* m_uiLayer = nullptr;
    };


} // namespace FluidStudio