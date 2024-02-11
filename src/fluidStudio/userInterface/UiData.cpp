#include "UiData.hpp"

#include "LibFluidAssert.hpp"

namespace FluidStudio
{

    FluidSolverWindow &UiData::window() const
    {
        FLUID_ASSERT(m_window != nullptr, "UiData was not initialized!");
        return *m_window;
    }

    UiElementCollection &UiData::collection() const
    {
        FLUID_ASSERT(m_collection != nullptr, "UiData was not initialized!");
        return *m_collection;
    }

    UiData::UiData(FluidSolverWindow *window, UiElementCollection &collection, UiLayer *uiLayer)
    {
        FLUID_ASSERT(window != nullptr);
        m_window = window;
        m_collection = &collection;
        m_uiLayer = uiLayer;
    }

    UiLayer &UiData::uiLayer() const
    {
        FLUID_ASSERT(m_uiLayer != nullptr, "UiLayer was not initialized!");
        return *m_uiLayer;
    }

    UiData::UiData() = default;

} // namespace FluidStudio
