#pragma once

#include "userInterface/UiElement.hpp"

#include <string>

namespace FluidStudio {
    class ObjImportWindow final : public UiElement {
      public:
        void update() override;

        void open_window();

      private:
        enum class Sampler{
            GridSampler,
            UvSampler
        };

      private:
        bool visible = false;

        std::string current_file;
        float import_scale = 1.0f;
        uint32_t particle_tag = 0;

        Sampler sampler = Sampler::UvSampler;

        bool duplicate_reduction_enabled = true;
        bool volume_reduction_enabled = true;
        bool distance_reduction_enabled = false;

        bool report_enabled = true;

        float get_particle_mass() const;

        void reset();

        void import_data_into_scene();

        bool can_import() const;

        struct ReportData {
            int created_particles = 0;
        } report_data;

        unsigned int update_report_popup();
        void show_report_popup(unsigned int id);
    };
} // namespace FluidStudio