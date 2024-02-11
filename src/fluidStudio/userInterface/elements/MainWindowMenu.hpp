#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio
{

    class MainWindowMenu final : public UiElement
    {
      public:
        void update() override;

      private:
        bool save_menu_open = false;

        void update_save_menu();
        void update_open_menu();
    };
} // namespace FluidStudio
