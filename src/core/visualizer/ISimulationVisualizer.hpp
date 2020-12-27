#ifndef FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
#define FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP

#include "core/fluidSolver/ParticleCollection.hpp"


namespace FluidSolver {


	class ISimulationVisualizer {
	public:

		struct Viewport {
			float left = -10.0f;
			float top = 10.0f;

			float right = 10.0f;
			float bottom = -10.0f;
		};

		struct Size {
			size_t width = 1920;
			size_t height = 1080;
		};


		struct VisualizerParameter {
			// viewport is in particle space. Everything inside the viewport should be visible.
			Viewport viewport;

			// this size specifies how large the render target should be in pixels
			Size render_target;

			ParticleCollection* collection = nullptr;
			float rest_density = 0.0f;
			float particle_size = 0.0f;

		} parameters;

		virtual void initialize() = 0;

		virtual void update_data() = 0;

		virtual void render() = 0;

		virtual ~ISimulationVisualizer() = default;

	};
}

#endif //FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
