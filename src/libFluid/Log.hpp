#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

namespace LibFluid {

    class Log {

      public:
        enum Type
        {
            TYPE_MESSAGE = 0,
            TYPE_WARNING = 1,
            TYPE_ERROR = 2
        };


        static std::string type_to_string(Type type);


        struct Entry
        {
            std::string message;
            std::chrono::system_clock::time_point time;
            Type type;
            std::string human_readable() const;
        };

        static void log(Type type, const std::string& message);

        static void message(const std::string& message);

        static void warning(const std::string& message);

        static void error(const std::string& message);

        static std::vector<Entry> get_entries();

        static bool print_to_console;

      private:
        static std::vector<Entry> entries;
        static std::mutex mutex;

        static void print_entry_to_console(const Entry& entry);
    };


} // namespace FluidSolver