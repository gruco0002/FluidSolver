#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include <string>

namespace LibFluid::Serialization {
    class SerializationContext {
      public:
        struct Issue{
            std::string message;
            std::vector<std::string> section_stack;
        };

      public:
        std::filesystem::path json_filepath;
        std::filesystem::path particle_data_relative_filepath;

        std::vector<Issue> issues;

        void begin_section(const std::string& name);
        void end_section();

        void add_issue(const std::string& message);

      private:
        std::vector<std::string> section_stack;

    };
} // namespace LibFluid::Serialization
