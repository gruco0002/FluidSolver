#include "Log.hpp"
#include <ctime>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>


namespace FluidSolver {

	std::vector<Log::Entry> Log::entries;
	std::mutex Log::mutex;

	bool Log::print_to_console = true;


	void Log::message(const std::string& message) {
		log(Type::TYPE_MESSAGE, message);
	}

	void Log::warning(const std::string& message) {
		log(Type::TYPE_WARNING, message);
	}

	void Log::error(const std::string& message) {
		log(Type::TYPE_ERROR, message);
	}

	std::vector<Log::Entry> Log::get_entries() {
		std::lock_guard<std::mutex> lock(mutex);
		return entries;
	}

	void Log::print_entry_to_console(const Entry& entry)
	{
		auto content = entry.human_readable();
		switch (entry.type)
		{
		case Type::TYPE_MESSAGE:
			fmt::print("{}\n", content);
			break;
		case Type::TYPE_WARNING:
			fmt::print(fg(fmt::terminal_color::yellow), "{}\n", content);
			break;
		case Type::TYPE_ERROR:
			fmt::print(fg(fmt::terminal_color::red), "{}\n", content);
			break;
		}

	}

	std::string Log::type_to_string(Type type)
	{
		switch (type)
		{
		case FluidSolver::Log::TYPE_MESSAGE:
			return "Message";
		case FluidSolver::Log::TYPE_WARNING:
			return "Warning";
		case FluidSolver::Log::TYPE_ERROR:
			return "Error";
		default:
			return "";
		}
	}

	void Log::log(Type type, const std::string& message) {
		Entry entry;
		entry.message = message;
		entry.time = std::chrono::system_clock::now();
		entry.type = type;

		if (print_to_console) {
			print_entry_to_console(entry);
		}

		std::lock_guard<std::mutex> lock(mutex);
		entries.push_back(entry);
	}



	std::string Log::Entry::human_readable() const
	{
		return fmt::format("{:%H:%M:%S}\t[{}]\t{}", time, type_to_string(type), message);
	}

}