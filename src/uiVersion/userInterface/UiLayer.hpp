#ifndef FLUIDSOLVER_UILAYER_HPP
#define FLUIDSOLVER_UILAYER_HPP

#include "StatisticsUi.hpp"
#include <uiVersion/userInterface/LogWindow.hpp>

namespace FluidUi {
	class FluidSolverWindow;

	class UiLayer {


	public:
		FluidSolverWindow* window = nullptr;
		void render();
		void initialize();


	private:
		StatisticsUi statisticsUi;
		LogWindow logWindow;

		void render_solver_parameters();


		void render_menu();
	};

}


#endif //FLUIDSOLVER_UILAYER_HPP
