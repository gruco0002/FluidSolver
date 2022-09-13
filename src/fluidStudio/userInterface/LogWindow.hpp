#pragma once

#include "userInterface/UiElement.hpp"
#include <string>
#include <vector>

namespace FluidStudio {

    class LogWindow final : public UiElement {
      public:
        void update() override;

      private:
        std::vector<std::string> messages;
        bool scroll_to_bottom = true;
    };

} // namespace FluidStudio
