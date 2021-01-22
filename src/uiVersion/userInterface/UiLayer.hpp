#ifndef FLUIDSOLVER_UILAYER_HPP
#define FLUIDSOLVER_UILAYER_HPP

#include "StatisticsUi.hpp"
#include <uiVersion/userInterface/LogWindow.hpp>

namespace FluidUi {
	class FluidSolverWindow;

	class UiLayer {
	private:
		struct Component {
			enum class Kind {
				None,
				Solver,
				Visualizer,
				Timestep,
				Sensor
			} kind = Kind::None;
			size_t index = 0;

			bool operator==(const Component& other) const;

			bool operator!=(const Component& other) const;


		} selection;

	public:
		FluidSolverWindow* window = nullptr;
		void render();
		void initialize();

	private:
		StatisticsUi statisticsUi;
		LogWindow logWindow;

		void render_component_panel();
		void render_simulation_controls();
		void render_menu();
		void render_component_properties_panel();

		void render_component_node(const char* name, const Component& component);
		void render_component_settings(const Component& component);
		void render_solver_component();
		void render_solver_parameters();
		void render_timestep_component();


	};

}


#endif //FLUIDSOLVER_UILAYER_HPP
