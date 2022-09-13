#pragma once

#include "FluidStudioFwd.hpp"
#include "StatisticsUi.hpp"
#include "TimelineUi.hpp"
#include "sensors/SensorPlane.hpp"
#include "userInterface/LogWindow.hpp"
#include "userInterface/MainWindowMenu.hpp"
#include "userInterface/PlyImportWindow.hpp"
#include "userInterface/SimulationControlsWindow.hpp"


namespace FluidStudio {


    class UiLayer {
      private:
        struct Component {
            enum class Kind {
                None,
                Solver,
                Visualizer,
                Timestep,
                Sensor,
                Output,
                Entity
            } kind = Kind::None;
            size_t index = 0;

            bool operator==(const Component& other) const;

            bool operator!=(const Component& other) const;

            bool can_delete() const;

        } selection;

      public:
        UiLayer();
        ~UiLayer();

        FluidSolverWindow* window = nullptr;
        void render();
        void initialize();

      private:
        StatisticsUi statisticsUi;


        TimelineUi timeline_ui;


        void render_component_panel();
        void render_component_properties_panel();

        void render_component_node(const char* name, const Component& component);
        void render_component_settings(const Component& component);
        void render_solver_component();
        void render_solver_parameters();
        void render_timestep_component();
        void render_sensor_component(size_t index);
        void render_entity_component(size_t index);
        void render_output_component();
        void render_visualizer_component();

        void update_selection_based_ui();

        const char* get_entity_type_name(const std::shared_ptr<LibFluid::IEntity> entity) const;


        void render_global_energy_sensor_component(std::shared_ptr<LibFluid::Sensors::GlobalEnergySensor> sen);
        void render_sensor_plane_component(std::shared_ptr<LibFluid::Sensors::SensorPlane> sen);
        void render_particle_remover_3d_component(std::shared_ptr<LibFluid::ParticleRemover3D> ent);

        void delete_component(const Component& component);

        std::vector<std::string> menu_entity_names;


      private:
        UiElementCollection ui_element_collection;

        SimulationControlsWindow simulation_controls_window;
        MainWindowMenu main_window_menu;
        PlyImportWindow ply_import_window;
        LogWindow log_window;
    };

} // namespace FluidStudio
