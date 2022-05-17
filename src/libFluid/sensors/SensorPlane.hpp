#pragma once

#include "sensors/ISensor.hpp"
#include "sensors/SensorData.hpp"
#include "visualizer/Image.hpp"

namespace FluidSolver::Sensors {

    class SensorPlane : public ISensor {
      public:
        enum class SensorPlaneType {
            SensorPlaneTypeDensity,
            SensorPlaneTypeVelocity,
            SensorPlaneTypePressure
        };

        struct SensorPlaneSettings {
            vec3 origin = vec3(0.0f, 0.0f, 0.0f);
            vec3 span_x = vec3(1.0f, 0.0f, 0.0f);
            vec3 span_y = vec3(0.0f, 1.0f, 0.0f);

            float width = 30.0f;
            float height = 10.0f;

            size_t number_of_samples_x = 300;
            size_t number_of_samples_y = 100;

            float max_image_value = 1.02f;
            float min_image_value = 0.98f;

            SensorPlaneType sensor_type = SensorPlaneType::SensorPlaneTypeDensity;

            size_t sub_sample_grid_size = 2;

            size_t calculate_plane_every_nth_step = 1;

        } settings;


        virtual void calculate_and_store(const Timepoint& timepoint) override;

        virtual void save_data_to_file(SensorWriter& writer) override;

        virtual void create_compatibility_report(CompatibilityReport& report) override;

        const Image& get_last_image() const;


      private:
        Image last_image = Image(1, 1);

        std::vector<vec3> last_values;

        Image get_image_representation() const;

        SensorData<std::string> data;
        size_t saved_data_until = 0;
    };

} // namespace FluidSolver::Sensors
