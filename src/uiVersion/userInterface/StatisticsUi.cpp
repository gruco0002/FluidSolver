#include "StatisticsUi.hpp"
#include "core/FluidInclude.hpp"
#include "uiVersion/ImguiHelper.hpp"
#include <implot.h>

void FluidUi::StatisticsUi::render() {
    ImGui::Begin("Statistics");

    if (ImPlot::BeginPlot("My Plot")) {

        ImPlot::PlotLine("Test", [](void *data, int x) {
            return ImPlotPoint(x, x);
        }, nullptr, 100);

        ImPlot::EndPlot();
    }

    ImGui::End();
}

void FluidUi::StatisticsUi::initialize() {
    FLUID_ASSERT(window != nullptr)
}
