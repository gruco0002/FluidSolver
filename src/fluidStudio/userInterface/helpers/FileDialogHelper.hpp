#pragma once

#include <optional>
#include <string>

namespace FluidStudio::FileDialogHelper
{

    std::optional<std::string> show_safe_file_dialog(const char *filter_list);

    std::optional<std::string> show_open_file_dialog(const char *filter_list);

}; // namespace FluidStudio::FileDialogHelper
