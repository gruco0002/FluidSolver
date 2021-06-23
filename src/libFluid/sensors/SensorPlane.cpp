#include "SensorPlane.hpp"

#include "Simulation.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "parallelization/StdParallelForEach.hpp"

namespace FluidSolver::Sensors
{

    using parallel = StdParallelForEach;

    void SensorPlane::initialize()
    {
    }

    void SensorPlane::calculate_and_store(const Timepoint& timepoint)
    {
        FLUID_ASSERT(parameters.neighborhood_interface != nullptr);
        FLUID_ASSERT(parameters.simulation_parameters != nullptr);
        FLUID_ASSERT(parameters.simulation_parameters->collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(parameters.simulation_parameters->collection->is_type_present<ParticleData>());

        last_values.clear();
        last_values.resize(settings.number_of_samples_x * settings.number_of_samples_y);

        vec3 span_x = glm::normalize(settings.span_x);
        vec3 span_y = glm::normalize(settings.span_y);
        float x_step = settings.width / settings.number_of_samples_x;
        float y_step = settings.height / settings.number_of_samples_y;

        CubicSplineKernel3D kernel;
        kernel.kernel_support = parameters.neighborhood_interface->get_search_radius();


        parallel::loop_for(0, settings.number_of_samples_x * settings.number_of_samples_y, [&](size_t i) {
            size_t y = i / settings.number_of_samples_x;
            size_t x = i % settings.number_of_samples_x;

            float value = 0.0f;
            vec3 sample_position = settings.origin + span_x * x_step + span_y * y_step;

            auto neighbors = parameters.neighborhood_interface->get_neighbors(sample_position);
            for (auto neighbor : neighbors)
            {
                auto& mv = parameters.simulation_parameters->collection->get<MovementData3D>(neighbor);
                auto& pd = parameters.simulation_parameters->collection->get<ParticleData>(neighbor);

                value += pd.density * kernel.GetKernelValue(mv.position, sample_position);
            }


            last_values[i] = value;
        });

        last_image = get_image_representation();
    }

    void SensorPlane::save_data_to_file(SensorWriter& writer)
    {
    }

    Compatibility SensorPlane::check()
    {
        Compatibility c;

        return c;
    }

    Image SensorPlane::get_image_representation() const
    {
        Image img(settings.number_of_samples_x, settings.number_of_samples_y);


        parallel::loop_for(0, settings.number_of_samples_x * settings.number_of_samples_y, [&](size_t i) {
            size_t y = i / settings.number_of_samples_x;
            size_t x = i % settings.number_of_samples_x;

            float value = 0.0f;

            if (last_values.size() > i)
            {
                value = last_values[i];
            }
            value -= settings.min_image_value;
            value = value / (settings.max_image_value - settings.min_image_value);

            if (value > 1.0f)
                value = 1.0f;
            if (value < 0.0f)
                value = 0.0f;

            uint8_t color = value * 255;

            img.set(x, y, Image::Color(color, color, color));
        });

        return img;
    }

    const Image& SensorPlane::get_last_image() const
    {
        return last_image;
    }

} // namespace FluidSolver::Sensors