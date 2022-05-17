#include "SensorPlane.hpp"

#include "Log.hpp"
#include "OutputManager.hpp"
#include "Simulator.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "parallelization/StdParallelForEach.hpp"

namespace FluidSolver::Sensors {

    using parallel = StdParallelForEach;


    void SensorPlane::calculate_and_store(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.neighborhood_interface != nullptr);
        FLUID_ASSERT(simulator_data.collection != nullptr);
        FLUID_ASSERT(simulator_data.collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(simulator_data.collection->is_type_present<ParticleData>());

        // check if we should even recalculate the plane
        if (timepoint.timestep_number % settings.calculate_plane_every_nth_step != 0) {
            return;
        }

        // clear the old array of data and resize accordingly
        last_values.clear();
        last_values.resize(settings.number_of_samples_x * settings.number_of_samples_y);

        // calculate the plane
        vec3 span_x = glm::normalize(settings.span_x);
        vec3 span_y = glm::normalize(settings.span_y);
        float x_step = settings.width / settings.number_of_samples_x;
        float y_step = settings.height / settings.number_of_samples_y;

        // initialize kernel for sph equations
        CubicSplineKernel3D kernel;
        kernel.kernel_support = simulator_data.neighborhood_interface->get_search_radius();
        kernel.initialize();

        // calculate in parallel every sample location
        parallel::loop_for(0, settings.number_of_samples_x * settings.number_of_samples_y, [&](size_t i) {
            size_t y = settings.number_of_samples_y - 1 - (i / settings.number_of_samples_x);
            size_t x = i % settings.number_of_samples_x;

            vec3 value = vec3(0.0f);

            for (size_t sub_x_count = 0; sub_x_count <= settings.sub_sample_grid_size; sub_x_count++) {
                float sub_x = sub_x_count / (settings.sub_sample_grid_size + 1.0f) - 0.5f;
                for (size_t sub_y_count = 0; sub_y_count <= settings.sub_sample_grid_size; sub_y_count++) {
                    float sub_y = sub_y_count / (settings.sub_sample_grid_size + 1.0f) - 0.5f;

                    vec3 sample_position =
                            settings.origin + span_x * x_step * (x + sub_x) + span_y * y_step * (y + sub_y);
                    auto neighbors = simulator_data.neighborhood_interface->get_neighbors(sample_position);
                    for (auto neighbor : neighbors) {
                        auto& mv = simulator_data.collection->get<MovementData3D>(neighbor);
                        auto& pd = simulator_data.collection->get<ParticleData>(neighbor);

                        float kernel_value = kernel.GetKernelValue(mv.position, sample_position);

                        switch (settings.sensor_type) {
                            case SensorPlaneType::SensorPlaneTypeDensity:
                                value += vec3(pd.mass, 0.0f, 0.0f) * kernel_value;
                                break;
                            case SensorPlaneType::SensorPlaneTypeVelocity:
                                value += mv.velocity * kernel_value;
                                break;
                            case SensorPlaneType::SensorPlaneTypePressure:
                                value += vec3(0.0f, 0.0f, pd.pressure) * kernel_value;
                                break;
                        }
                    }
                }
            }

            value /= Math::pow2(settings.sub_sample_grid_size + 1);
            last_values[i] = value;
        });

        // convert data into an image representation
        last_image = get_image_representation();

        // save the data to file and add an entry into the sensor data if required
        if (parameters.save_to_file) {
            // calculate the filename
            auto filename = std::to_string(timepoint.timestep_number) + ".png";

            // save the image to a file
            FLUID_ASSERT(simulator_data.manager != nullptr);
            auto path = simulator_data.manager->get_filepath_for_sensor(this, filename);
            if (path.has_value()) {
                last_image.save_as_png(path.value().string());
                data.push_back(timepoint, filename);
            } else {
                Log::warning("[SensorPlane] Could not save image output of sensor plane!");
            }
        }
    }

    void SensorPlane::save_data_to_file(SensorWriter& writer) {
        if (writer.begin_header()) {
            writer.push_back_header<Timepoint>("Timepoint");
            writer.push_back_header<std::string>("Filename");
            writer.end_header();
        }


        // data writing
        for (size_t i = saved_data_until; i < data.size(); i++) {
            auto& tmp = data.data()[i];
            writer << data.times()[i] << tmp << SensorWriter::Control::Next;
        }


        if (parameters.keep_data_in_memory_after_saving) {
            saved_data_until = data.size();
        } else {
            data.clear();
            saved_data_until = 0;
        }
    }

    void SensorPlane::create_compatibility_report(CompatibilityReport& report) {
    }

    Image SensorPlane::get_image_representation() const {
        Image img(settings.number_of_samples_x, settings.number_of_samples_y);


        parallel::loop_for(0, settings.number_of_samples_x * settings.number_of_samples_y, [&](size_t i) {
            size_t y = i / settings.number_of_samples_x;
            size_t x = i % settings.number_of_samples_x;

            vec3 value = vec3(0.0f);

            if (last_values.size() > i) {
                value = last_values[i];
            }
            value -= vec3(settings.min_image_value);
            value = value / vec3(settings.max_image_value - settings.min_image_value);

            if (value.x > 1.0f)
                value.x = 1.0f;
            if (value.x < 0.0f)
                value.x = 0.0f;

            if (value.y > 1.0f)
                value.y = 1.0f;
            if (value.y < 0.0f)
                value.y = 0.0f;

            if (value.z > 1.0f)
                value.z = 1.0f;
            if (value.z < 0.0f)
                value.z = 0.0f;

            uint8_t color_r = value.x * 255;
            uint8_t color_g = value.y * 255;
            uint8_t color_b = value.z * 255;

            img.set(x, y, Image::Color(color_r, color_g, color_b));
        });

        return img;
    }

    const Image& SensorPlane::get_last_image() const {
        return last_image;
    }

} // namespace FluidSolver::Sensors