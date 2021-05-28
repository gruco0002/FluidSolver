#pragma once

#include "core/FluidInclude.hpp"
#include "core/timestep/ITimestep.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace FluidSolver
{

    class SensorWriter {

      public:
        enum class Control
        {
            Next,
        };

        const char* SEPERATOR = ";";
        const char* NEWLINE = "\r\n";

      private:
        bool in_header_mode = true;
        size_t header_count = 0;
        std::vector<size_t> dimensionality_info;
        size_t current_row_position = 0;
        std::string filepath;
        std::ofstream stream;

        void next_value();
        void add_seperator_if_required();
        void check_correct_dimensionality_at_position(size_t dimensionality) const;

      public:
        explicit SensorWriter(const std::string& filepath);
        ~SensorWriter();

        bool begin_header() const;
        void push_back_header(const std::string& header, size_t dimensionality);
        void push_back_header(std::initializer_list<std::string> headers);
        void end_header();

        template <typename T> void push_back_header(const std::string& header)
        {
            FLUID_ASSERT(false);
        }


        void push_back(float value);
        void push_back(const std::string& value);
        void push_back(const glm::vec2& value);
        void push_back(const glm::vec3& value);
        void push_back(const glm::vec4& value);
        void push_back(int value);
        void push_back(size_t value);
        void push_back(const FluidSolver::Timepoint& value);

        void next();

        SensorWriter& operator<<(float value);
        SensorWriter& operator<<(const std::string& value);
        SensorWriter& operator<<(const glm::vec2& value);
        SensorWriter& operator<<(const glm::vec3& value);
        SensorWriter& operator<<(const glm::vec4& value);
        SensorWriter& operator<<(int value);
        SensorWriter& operator<<(size_t value);
        SensorWriter& operator<<(const FluidSolver::Timepoint& value);

        SensorWriter& operator<<(Control control);
    };


} // namespace FluidSolver
