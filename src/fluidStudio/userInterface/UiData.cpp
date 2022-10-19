#include "UiData.hpp"

#include "LibFluidAssert.hpp"

namespace FluidStudio {

    FluidSolverWindow& UiData::window() const {
        FLUID_ASSERT(m_window != nullptr, "UiData was not initialized!");
        return *m_window;
    }
    UiElementCollection& UiData::collection() const {
        FLUID_ASSERT(m_collection != nullptr, "UiData was not initialized!");
        return *m_collection;
    }
    UiData::UiData(FluidSolverWindow* window, UiElementCollection& collection) {
        FLUID_ASSERT(window != nullptr);
        m_window = window;
        m_collection = &collection;
    }
    UiData::UiData() = default;


} // namespace FluidStudio