#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio {

    class MainWindowMenu final : public UiElement {
      public:
        void update() override;

      private:
        bool save_menu_open = false;
        bool tag_selection_modal_open = false;

        void update_save_menu();
        void update_open_menu();
        void update_tag_selection_modal();

        bool is_3d_simulation() const;
    };
} // namespace FluidStudio
