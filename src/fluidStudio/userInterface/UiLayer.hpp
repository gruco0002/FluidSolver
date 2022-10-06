#pragma once

#include "userInterface/elements/ComponentPanelWindow.hpp"
#include "userInterface/elements/ComponentSettingsWindow.hpp"
#include "userInterface/elements/LogWindow.hpp"
#include "userInterface/elements/MainWindowMenu.hpp"
#include "userInterface/elements/ObjImportWindow.hpp"
#include "userInterface/elements/PlyImportWindow.hpp"
#include "userInterface/elements/SensorGraphWindows.hpp"
#include "userInterface/elements/SimulationControlsWindow.hpp"
#include "userInterface/elements/TimelineWindow.hpp"
#include "userInterface/elements/NewSimulationModalWindow.hpp"


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
        ObjImportWindow obj_import_window;
        NewSimulationModalWindow new_simulation_modal_window;
    };

} // namespace FluidStudio
