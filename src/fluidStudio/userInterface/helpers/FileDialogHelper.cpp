#include "FileDialogHelper.hpp"

#include <nfd.h>

namespace FluidStudio::FileDialogHelper {


    std::optional<std::string> show_safe_file_dialog(const char* filter_list) {
        char* p = nullptr;

        auto res = NFD_SaveDialog(filter_list, nullptr, &p);
        if (res == NFD_OKAY) {
            std::string result(p);
            free(p);
            return result;
        } else {
            free(p);
        }
        return {};
    }

    std::optional<std::string> show_open_file_dialog(const char* filter_list) {
        char* p = nullptr;

        auto res = NFD_OpenDialog(filter_list, nullptr, &p);
        if (res == NFD_OKAY) {
            std::string path(p);
            free(p);
            return path;
        } else {
            free(p);
        }
        return {};
    }

} // namespace FluidStudio::FileDialogHelper