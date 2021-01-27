#pragma once

#include <string>
#include <iostream>
#include <fstream>

#include "core/FluidInclude.hpp"

namespace FluidSolver {

	class SensorWriter {

	public:
		enum class Control {
			Next,
		};

	private:
		bool in_header_mode = true;
		size_t header_count = 0;
        std::string filepath;
        std::ofstream stream;

	public:
        
        explicit SensorWriter(const std::string& filepath);

		void push_back_header(const std::string& header, size_t dimensionality = 1);
		void end_header();

		void push_back(float value);
		void push_back(const std::string& value);
		void push_back(const glm::vec2& value);
		void push_back(const glm::vec3& value);
		void push_back(const glm::vec4& value);
		void push_back(int value);

		void next();

		SensorWriter& operator<<(float value);
		SensorWriter& operator<<(const std::string& value);
		SensorWriter& operator<<(const glm::vec2& value);
		SensorWriter& operator<<(const glm::vec3& value);
		SensorWriter& operator<<(const glm::vec4& value);
		SensorWriter& operator<<(int value);
		
		SensorWriter& operator<<(Control control);

	};



}
