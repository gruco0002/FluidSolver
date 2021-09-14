#include "OutputManager.hpp"

#include <filesystem>
#include <set>


void FluidSolver::OutputManager::initialize()
{
}

void FluidSolver::OutputManager::timestep_happened()
{
    if (timesteps_since_last_sensor_save >= parameters.timesteps_between_sensor_save)
    {
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
    for (auto& [key, value] : sensor_writers)
    {
        value = nullptr;
    }
    sensor_writers.clear();
}

void FluidSolver::OutputManager::save_sensor_data()
{
    // creating output directory if it does not exist
    if (!std::filesystem::exists(parameters.output_folder))
    {
        std::filesystem::create_directories(parameters.output_folder);
    }

    // reset timesteps since last sensor save
    timesteps_since_last_sensor_save = 0;

    {
        // remove unneeded sensor writers
        std::set<std::shared_ptr<FluidSolver::ISensor>> sensor_set(parameters.sensors.begin(),
                                                                   parameters.sensors.end());
        std::vector<std::shared_ptr<FluidSolver::ISensor>> to_remove;
        for (auto& [key, value] : sensor_writers)
        {
            auto res = sensor_set.find(key);
            if (res == sensor_set.end() || !(*res)->parameters.save_to_file)
            {
                to_remove.push_back(key);
                value = nullptr;
            }
        }
        for (auto sen : to_remove)
        {
            sensor_writers.erase(sen);
        }
    }


    // check for the writers being present
    for (auto sen : parameters.sensors)
    {
        if (!sen->parameters.save_to_file)
            continue;
        auto res = sensor_writers.find(sen);
        if (res == sensor_writers.end())
        {
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
        writer.flush();
    }
}

std::string FluidSolver::OutputManager::remove_invalid_chars_from_filename(const std::string& filename)
{
    static const std::string invalid_chars = "\\/:?\"<>|";

    std::string res;
    for (auto it = filename.begin(); it != filename.end(); ++it)
    {
        bool found = invalid_chars.find(*it) != std::string::npos;
        if (found)
        {
            res.append(" ", 1);
        }
        else
        {
            const char append = *it;
            res.append(1, append);
        }
    }
    return res;
}

std::optional<std::filesystem::path> FluidSolver::OutputManager::get_filepath_for_sensor(const ISensor* sensor,
                                                                                         std::string desired_filename)
{
    // creating output directory if it does not exist
    if (!std::filesystem::exists(parameters.output_folder))
    {
        std::filesystem::create_directories(parameters.output_folder);
    }

    if (!sensor->parameters.save_to_file)
    {
        // it is not allowed to save a file
        return {};
    }

    auto sensor_directory =
        std::filesystem::path(parameters.output_folder) / remove_invalid_chars_from_filename(sensor->parameters.name);

    // create sensor directory if required
    if (!std::filesystem::exists(sensor_directory))
    {
        std::filesystem::create_directories(sensor_directory);
    }

    // create filepath
    return sensor_directory / desired_filename;
}