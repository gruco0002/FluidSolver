#pragma once

#include "userInterface/UiElement.hpp"

#include <string>

namespace FluidStudio {
    class ObjImportWindow final : public UiElement {
      public:
        void update() override;

        void open_window();

      private:
        bool visible = false;

        std::string current_file;
        float import_scale = 1.0f;
        uint32_t particle_tag = 0;

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