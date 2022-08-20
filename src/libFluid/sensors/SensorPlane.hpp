#pragma once

#include "sensors/SensorBase.hpp"
#include "visualizer/Image.hpp"

namespace LibFluid::Sensors {

    // TODO: make an abstract class in terms of output dimensionality and unit
    class SensorPlane : public SensorBase<std::vector<glm::vec3>> {
      public:
        enum class SensorPlaneType {
            SensorPlaneTypeDensity,
            SensorPlaneTypeVelocity,
            SensorPlaneTypePressure
        };

        struct SensorPlaneSettings {
            glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 span_x = glm::vec3(1.0f, 0.0f, 0.0f);
            glm::vec3 span_y = glm::vec3(0.0f, 1.0f, 0.0f);

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


        std::vector<SensorDataFieldDefinition> get_definitions() override;
        std::vector<glm::vec3> calculate_for_timepoint(const Timepoint& timepoint) override;
        void add_data_fields_to_json_array(nlohmann::json& array, const std::vector<glm::vec3>& data) override;


        void create_compatibility_report(CompatibilityReport& report) override;

        const Image& get_last_image() const;

      private:
        Image last_image = Image(1, 1);

        Image get_image_representation(const std::vector<glm::vec3>& data) const;
    };

} // namespace LibFluid::Sensors
