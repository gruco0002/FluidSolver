#pragma once

#include <string>

namespace FluidStudio
{
    class DirectoryHelper
    {
      public:
        static const char *studio_directory();
        static const char *config_file();
        static const char *imgui_config_file();

      private:
        static std::string s_studio_directory;
        static std::string s_config_file;
        static std::string s_imgui_config_file;
        static bool loaded;

        static void load_data_if_required();
        static std::string get_home_directory();
    };
} // namespace FluidStudio
