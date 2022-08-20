#include "SensorPlane.hpp"

#include "Log.hpp"
#include "OutputManager.hpp"
#include "Simulator.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"
#include "parallelization/StdParallelForEach.hpp"
#include "serialization/Base64.hpp"
#include "serialization/JsonHelpers.hpp"

namespace LibFluid::Sensors {

    using parallel = StdParallelForEach;


    void SensorPlane::create_compatibility_report(CompatibilityReport& report) {
    }

    Image SensorPlane::get_image_representation(const std::vector<glm::vec3>& data) const {
        Image img(settings.number_of_samples_x, settings.number_of_samples_y);


        parallel::loop_for(0, settings.number_of_samples_x * settings.number_of_samples_y, [&](size_t i) {
            size_t y = i / settings.number_of_samples_x;
            size_t x = i % settings.number_of_samples_x;

            vec3 value = vec3(0.0f);

            if (data.size() > i) {
                value = data[i];
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

    std::vector<SensorDataFieldDefinition> SensorPlane::get_definitions() {
        return {
                {"Sensor values",
                        SensorDataFieldDefinition::FieldType::Custom,
                        "Matrix of the sensor values (3d-vectors) in row major notation.",
                        ""},
                {"Generated Image",
                        SensorDataFieldDefinition::FieldType::String,
                        "Base64 encoded png image of the sensor data",
                        ""}};
    }

    std::vector<glm::vec3> SensorPlane::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(simulator_data.neighborhood_interface != nullptr);
        FLUID_ASSERT(simulator_data.collection != nullptr);
        FLUID_ASSERT(simulator_data.collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(simulator_data.collection->is_type_present<ParticleData>());


        // clear the old array of data and resize accordingly
        std::vector<glm::vec3> output;
        output.resize(settings.number_of_samples_x * settings.number_of_samples_y);

        // calculate the plane
        glm::vec3 span_x = glm::normalize(settings.span_x);
        glm::vec3 span_y = glm::normalize(settings.span_y);
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

            glm::vec3 value = glm::vec3(0.0f);

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
                                value += glm::vec3(pd.mass, 0.0f, 0.0f) * kernel_value;
                                break;
                            case SensorPlaneType::SensorPlaneTypeVelocity:
                                value += mv.velocity * kernel_value;
                                break;
                            case SensorPlaneType::SensorPlaneTypePressure:
                                value += glm::vec3(0.0f, 0.0f, pd.pressure) * kernel_value;
                                break;
                        }
                    }
                }
            }

            value /= Math::pow2(settings.sub_sample_grid_size + 1);
            output[i] = value;
        });

        // convert data into an image representation
        last_image = get_image_representation(output);

        return output;
    }

    void SensorPlane::add_data_fields_to_json_array(nlohmann::json& array, const std::vector<glm::vec3>& data) {
        // add the data values to the output array
        auto a = nlohmann::json::array();
        for (const auto& value : data) {
            a.push_back(value);
        }
        array.push_back(a);

        // generate an image as png, encoded it as base64 string and add it to the array
        auto image = get_image_representation(data);
        auto binary_image_data = image.get_as_png();
        auto base64_image_data = Base64::encode_to_base64(binary_image_data);
        array.push_back(base64_image_data);
    }

} // namespace LibFluid::Sensors