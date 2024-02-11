#include "DirectoryHelper.hpp"

#include <filesystem>

namespace FluidStudio {

    std::string DirectoryHelper::s_studio_directory;
    std::string DirectoryHelper::s_config_file;
    std::string DirectoryHelper::s_imgui_config_file;
    bool DirectoryHelper::loaded = false;

    const char* DirectoryHelper::studio_directory() {
        load_data_if_required();
        return s_studio_directory.c_str();
    }
    const char* DirectoryHelper::config_file() {
        load_data_if_required();
        return s_config_file.c_str();
    }
    const char* DirectoryHelper::imgui_config_file() {
        load_data_if_required();
        return s_imgui_config_file.c_str();
    }

    void DirectoryHelper::load_data_if_required() {
        if (loaded)
            return;
        loaded = true;

        auto home_directory = get_home_directory();
        s_studio_directory = (std::filesystem::path(home_directory) / std::string(".FluidStudio/")).string();
        s_config_file = (std::filesystem::path(home_directory) / std::string(".FluidStudio/config.json")).string();
        s_imgui_config_file = (std::filesystem::path(home_directory) / std::string(".FluidStudio/imgui.ini")).string();


        std::filesystem::create_directories(s_studio_directory);
    }

    std::string DirectoryHelper::get_home_directory() {
        return "./";
    }
} // namespace FluidStudio