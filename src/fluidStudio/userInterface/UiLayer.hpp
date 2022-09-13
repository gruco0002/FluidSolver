#pragma once

#include "userInterface/ComponentPanelWindow.hpp"
#include "userInterface/ComponentSettingsWindow.hpp"
#include "userInterface/LogWindow.hpp"
#include "userInterface/MainWindowMenu.hpp"
#include "userInterface/PlyImportWindow.hpp"
#include "userInterface/SensorGraphWindows.hpp"
#include "userInterface/SimulationControlsWindow.hpp"
#include "userInterface/TimelineWindow.hpp"


namespace FluidStudio {


    class UiLayer {
      public:
        UiLayer();
        ~UiLayer();

        FluidSolverWindow* window = nullptr;
        void render();
        void initialize();


      private:
        UiElementCollection ui_element_collection;

        SimulationControlsWindow simulation_controls_window;
        MainWindowMenu main_window_menu;
        PlyImportWindow ply_import_window;
        LogWindow log_window;
        TimelineWindow timeline_window;
        SensorGraphWindows sensor_graph_windows;
        ComponentPanelWindow component_panel_window;
        ComponentSettingsWindow component_settings_window;
    };

} // namespace FluidStudio
