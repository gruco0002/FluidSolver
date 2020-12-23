#ifndef FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP
#define FLUIDSOLVER_ISIMULATIONVISUALIZER_HPP

#include "core/fluidSolver/ParticleCollection.hpp"


namespace FluidSolver {


	class ISimulationVisualizer {
	public:

		struct Viewport {
			float left = 0.0f;
			float top = 0.0f;

			float right = 0.0f;
			float bottom = 0.0f;
		};

		struct Size {
			size_t width = 0;
			size_t height = 0;
		};


		struct VisualizerParameter {
			// viewport is in particle space. Everything inside the viewport should be visible.
			Viewport viewport;

			// this size specifies how large the render target should be in pixels
			Size render_targer;

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
