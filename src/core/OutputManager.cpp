#include "OutputManager.hpp"
#include <set>
#include <filesystem>


void FluidSolver::OutputManager::initialize()
{
}

void FluidSolver::OutputManager::timestep_happened()
{
	if (timesteps_since_last_sensor_save >= parameters.timesteps_between_sensor_save) {
		save_sensor_data();
	}
	timesteps_since_last_sensor_save++;

}

void FluidSolver::OutputManager::manual_save()
{
	save_sensor_data();
}

FluidSolver::OutputManager::~OutputManager()
{
	// close and delete sensor writers
	for (auto& [key, value] : sensor_writers) {
		delete value;
		value = nullptr;
	}
	sensor_writers.clear();
}

void FluidSolver::OutputManager::save_sensor_data()
{
	// reset timesteps since last sensor save
	timesteps_since_last_sensor_save = 0;

	{
		// remove unneeded sensor writers
		std::set<FluidSolver::ISensor*> sensor_set(parameters.sensors.begin(), parameters.sensors.end());
		std::vector<FluidSolver::ISensor*> to_remove;
		for (auto& [key, value] : sensor_writers) {
			if (sensor_set.find(key) == sensor_set.end()) {
				to_remove.push_back(key);
				delete value;
				value = nullptr;
			}
		}
		for (auto sen : to_remove)
		{
			sensor_writers.erase(sen);
		}
	}


	// check for the writers being present	
	for (auto sen : parameters.sensors) {
		auto res = sensor_writers.find(sen);
		if (res == sensor_writers.end()) {
			// create a new sensor writer
			std::filesystem::path p(parameters.output_folder);
			p = p / remove_invalid_chars_from_filename(sen->parameters.name);

			auto writer = new SensorWriter(p.string() + ".csv");

			// add it to the map
			sensor_writers.emplace(sen, writer);

			// set the iterator 
			res = sensor_writers.find(sen);
		}

		// collect the data
		auto& writer = *res->second;
		sen->save_data_to_file(writer);

	}

}

std::string FluidSolver::OutputManager::remove_invalid_chars_from_filename(const std::string& filename)
{
	static const std::string invalid_chars = "\\/:?\"<>|";

	std::string res;
	for (auto it = filename.begin(); it != filename.end(); ++it) {
		bool found = invalid_chars.find(*it) != std::string::npos;
		if (found) {
			res.append(" ", 1);
		}
		else {
			const char append = *it;
			res.append(1, append);
		}
	}
	return res;
}
