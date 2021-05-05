#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/sensors/ISensor.hpp"

namespace FluidSolver {

	class OutputManager {

	public:

		struct OutputManagerParameters {
			std::string output_folder = "output/";

			size_t timesteps_between_sensor_save = 10;
			std::vector<ISensor*> sensors;
		} parameters;

		void initialize();

		void timestep_happened();

		void manual_save();

		~OutputManager();

	private:
		size_t timesteps_since_last_sensor_save = 0;

		std::map<ISensor*, SensorWriter*> sensor_writers;

		void save_sensor_data();

		static std::string remove_invalid_chars_from_filename(const std::string& filename);

		


	};


}