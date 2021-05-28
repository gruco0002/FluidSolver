#ifndef FLUIDSOLVER_CONTINOUSVISUALIZER_HPP
#define FLUIDSOLVER_CONTINOUSVISUALIZER_HPP

#include "core/visualizer/ISimulationVisualizer.hpp"
#include <vector>
#include "core/fluidSolver/kernel/CubicSplineKernel.hpp"
#include "core/visualizer/Viewport2D.hpp"

namespace FluidSolver {
	class ContinousVisualizer : public ISimulationVisualizer {

	public:

		ContinousVisualizer();

		struct Settings {
			Viewport2D viewport;
			Image::Color clear_color = Image::Color(0, 0, 0);
			float minimum_render_density = 0.0f;
		} settings;


		virtual void initialize() override;

		virtual void render() override;

		virtual Image get_image_data() override;



	private:

		Image image;

		Viewport2D internal_viewport;

		CubicSplineKernel kernel;

		void recalculate_viewport();

		Image::Color calculate_color_for_pixel(size_t x, size_t y);

		glm::vec2 calculate_particle_space_position_for_pixel(size_t x, size_t y);



	};
}


#endif //FLUIDSOLVER_CONTINOUSVISUALIZER_HPP
